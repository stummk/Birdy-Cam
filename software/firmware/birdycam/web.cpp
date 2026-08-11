#include "web.h"
#include "config.h"
#include "speicher.h"
#include "strom.h"
#include "audio.h"
#include "lichtschranke.h"
#include "avi.h"
#include "systeminfo.h"
#include "netzwerk.h"
#include <WebServer.h>
#include <WiFi.h>
#include <SD.h>

extern bool  istNacht;        // aus birdycam.ino
extern float aktuelleFps;

static WebServer server(80);
static WiFiServer streamServer(81);
static WiFiClient streamClient;

// ---------------------------------------------------------------------------
//  Der Ton für den Livestream läuft über eine eigene Tür (82).
//
//  Warum getrennt vom Bild: Ein Browser zeigt MJPEG in einem <img>-Element
//  an, und ein <img> kann keinen Ton. Also liefern wir den Ton als zweiten
//  Strom, den ein <audio>-Element abspielt — technisch eine WAV-Datei, die
//  einfach nie zu Ende geht. Im Kopf steht eine riesige Länge; der Browser
//  spielt, was ankommt, und wartet auf den Rest.
//
//  Der Preis dieser Einfachheit: Bild und Ton laufen etwa eine Sekunde
//  auseinander, weil der Browser den Ton puffert. Lippensynchron bekommt
//  man das nur mit WebRTC — und das passt nicht in einen ESP32.
// ---------------------------------------------------------------------------
static WiFiServer tonServer(82);
static WiFiClient tonClient;
static int16_t    tonAus[1024];       // 2 KB Zwischeneimer für den Tonstrom
static void       tonBedienen();      // steht weiter unten

// ---------------------------------------------------------------------------
//  Die Website. Steht als Text direkt im Programm (PROGMEM = im Flash,
//  nicht im knappen Arbeitsspeicher).
// ---------------------------------------------------------------------------
static const char SEITE[] PROGMEM = R"HTML(<!doctype html>
<html lang="de"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>BirdyCam</title>
<style>
:root{--bg:#14181d;--fg:#e8eef4;--k:#1d242c;--a:#5cc8a8;--b:#f0a94c;--m:#8a97a5}
@media(prefers-color-scheme:light){:root{--bg:#f4f6f8;--fg:#1a2028;--k:#fff;--m:#5d6b7a}}
*{box-sizing:border-box}
body{margin:0;font:16px/1.5 system-ui,sans-serif;background:var(--bg);color:var(--fg)}
header{padding:14px 16px;display:flex;align-items:center;gap:10px;flex-wrap:wrap}
h1{font-size:20px;margin:0;flex:1}
.pill{background:var(--k);padding:4px 10px;border-radius:99px;font-size:13px;color:var(--m)}
main{padding:0 16px 40px;max-width:820px;margin:auto}
.karte{background:var(--k);border-radius:14px;padding:14px;margin-bottom:14px}
.karte h2{font-size:15px;margin:0 0 10px;color:var(--m);font-weight:600;
  text-transform:uppercase;letter-spacing:.05em}
img#live{width:100%;border-radius:10px;background:#000;display:block;aspect-ratio:4/3;
  object-fit:contain}
.zahlen{display:grid;grid-template-columns:repeat(auto-fit,minmax(90px,1fr));gap:10px}
.z{background:var(--bg);border-radius:10px;padding:10px;text-align:center}
.z b{display:block;font-size:24px;color:var(--a)}
.z span{font-size:12px;color:var(--m)}
.balken{display:flex;align-items:flex-end;gap:2px;height:90px;margin-top:4px}
.balken div{flex:1;background:var(--a);border-radius:2px 2px 0 0;min-height:2px;
  position:relative}
.balken div.n{background:#3a4652}
.stdachse{display:flex;gap:2px;font-size:9px;color:var(--m);margin-top:3px}
.stdachse span{flex:1;text-align:center}
.akku{height:22px;background:var(--bg);border-radius:99px;overflow:hidden;position:relative}
.akku i{display:block;height:100%;background:var(--a);transition:width .4s}
.akku u{position:absolute;inset:0;text-align:center;font-size:13px;line-height:22px;
  text-decoration:none}
nav{display:flex;gap:8px;margin-bottom:10px;flex-wrap:wrap}
button{background:var(--bg);color:var(--fg);border:1px solid #33404d;padding:7px 14px;
  border-radius:99px;font-size:14px;cursor:pointer}
button.aktiv{background:var(--a);color:#0a1014;border-color:var(--a)}
ul{list-style:none;padding:0;margin:0;max-height:340px;overflow-y:auto}
li a{display:flex;justify-content:space-between;gap:8px;padding:9px 10px;
  color:var(--fg);text-decoration:none;border-radius:8px}
li a:hover{background:var(--bg)}
li small{color:var(--m);white-space:nowrap}
footer{text-align:center;color:var(--m);font-size:12px;padding:10px}
.info{margin:10px 0 0;color:var(--m);font-size:13px}
.ampel{padding:9px 12px;border-radius:10px;font-size:14px;font-weight:600;
  border:1px solid var(--rand,#33404d)}
.ampel.gut{background:rgba(92,200,168,.12);color:var(--a);border-color:var(--a)}
.ampel.warn{background:rgba(240,169,76,.12);color:var(--b);border-color:var(--b)}
.ampel.bad{background:rgba(224,103,79,.14);color:#e0674f;border-color:#e0674f}
.meldungen{list-style:none;padding:0;margin:8px 0 0;max-height:none;overflow:visible}
.meldungen li{padding:3px 0;font-size:13px;color:var(--m)}
.dtl{width:100%;border-collapse:collapse;font-size:12px;margin-top:8px}
.dtl td{padding:3px 0;color:var(--m);vertical-align:top}
.dtl td:first-child{width:52%}
.dtl td:last-child{color:var(--fg);text-align:right;font-variant-numeric:tabular-nums}
</style></head><body>
<header>
  <h1>&#128038; BirdyCam</h1>
  <span class="pill" id="modus">&hellip;</span>
  <span class="pill" id="betrieb">&hellip;</span>
  <span class="pill" id="akkupille">&hellip;</span>
  <span class="pill" id="vogel">&hellip;</span>
  <span class="pill" id="uhr">&hellip;</span>
</header>
<main>

<div class="karte">
  <h2>Livebild</h2>
  <img id="live" alt="Livebild">
  <nav style="margin:10px 0 0">
    <button id="tonbtn" onclick="tonUm()">&#128266; Ton an</button>
  </nav>
  <p class="info" id="toninfo"></p>
  <audio id="ton" hidden></audio>
</div>

<div class="karte">
  <h2>Heute</h2>
  <div class="zahlen">
    <div class="z"><b id="bh">0</b><span>Besuche heute</span></div>
    <div class="z"><b id="bg">0</b><span>Besuche gesamt</span></div>
    <div class="z"><b id="eh">--:--</b><span>erster Anflug</span></div>
    <div class="z"><b id="lb">--:--</b><span>letzter Anflug</span></div>
  </div>
</div>

<div class="karte">
  <h2>Wann ist Rushhour?</h2>
  <nav>
    <button class="aktiv" id="rh0" onclick="rushhour(0)">Heute</button>
    <button id="rh1" onclick="rushhour(1)">&Oslash; letzte Tage</button>
  </nav>
  <div class="balken" id="balken"></div>
  <div class="stdachse" id="achse"></div>
</div>

<div class="karte" id="verlaufkarte">
  <h2>Verlauf &mdash; letzte <span id="tageN">30</span> Tage</h2>
  <div class="balken" id="tbalken"></div>
  <div class="stdachse" id="tachse"></div>
  <p class="info" id="tinfo">&hellip;</p>
  <p class="info"><a href="/tage.csv" style="color:var(--a)">tage.csv
    herunterladen</a> &middot; eine Zeile je Tag, &ouml;ffnet sich in Excel</p>
</div>

<div class="karte">
  <h2>Akku</h2>
  <div class="akku"><i id="ab"></i><u id="at"></u></div>
  <p class="info" id="akkuinfo">&hellip;</p>
</div>

<div class="karte">
  <h2>Systemzustand</h2>
  <div id="ampel" class="ampel">&hellip;</div>
  <ul class="meldungen" id="meldungen"></ul>
  <p class="info" id="netzinfo">&hellip;</p>
  <div class="zahlen" style="margin-top:12px">
    <div class="z"><b id="s-wlan">?</b><span>WLAN-Signal</span></div>
    <div class="z"><b id="s-temp">?</b><span>Chip-Temperatur</span></div>
    <div class="z"><b id="s-heap">?</b><span>Arbeitsspeicher frei</span></div>
    <div class="z"><b id="s-psram">?</b><span>PSRAM frei</span></div>
  </div>
  <p class="info" id="sdinfo">&hellip;</p>
  <p class="info" id="karteinfo">&hellip;</p>
  <details style="margin-top:10px">
    <summary style="cursor:pointer;color:var(--m);font-size:13px">
      Technische Details
    </summary>
    <table class="dtl" id="details"></table>
  </details>
</div>

<div class="karte">
  <h2>Aufnahmen</h2>
  <nav>
    <button class="aktiv" onclick="lade('clips',this)">Videoclips</button>
    <button onclick="lade('fotos',this)">Fotos</button>
    <button onclick="lade('audio',this)">Vogelgesang</button>
  </nav>
  <ul id="liste"></ul>
</div>

</main>
<footer>BirdyCam &middot; l&auml;uft seit <span id="up">?</span></footer>
<script>
const $=i=>document.getElementById(i);
let stdHeute=[], verlauf=null, rhModus=0, tageN=0, tonAn=false;
// Der Livestream läuft auf Tür 81 — location.hostname klappt sowohl im
// Heimnetz (birdycam.local) als auch im eigenen WLAN (192.168.4.1).
$('live').src='http://'+location.hostname+':81/';

// Der Kamera die Uhrzeit schenken. Im eigenen WLAN hat sie kein Internet
// und kennt die Zeit sonst nicht — dann wären alle Zeitstempel falsch und
// das Stunden-Diagramm sinnlos. Die Kamera nimmt den Wert nur an, wenn sie
// noch keine Zeit hat.
fetch('/api/zeit?ts='+Math.floor(Date.now()/1000)).catch(()=>{});

function zeit(s){if(!s)return'--:--';const d=new Date(s*1000);
  return String(d.getHours()).padStart(2,'0')+':'+String(d.getMinutes()).padStart(2,'0');}
function kb(b){return b>1048576?(b/1048576).toFixed(1)+' MB':Math.round(b/1024)+' KB';}

async function status(){
  try{
    const s=await(await fetch('/api/status')).json();
    $('bh').textContent=s.besucheHeute; $('bg').textContent=s.besucheGesamt;
    $('eh').textContent=zeit(s.ersterHeute); $('lb').textContent=zeit(s.letzterBesuch);
    $('modus').textContent=s.nacht?'\u{1F319} Nacht':'☀️ Tag';
    $('betrieb').textContent=s.zuschauer
      ? '\u{1F4F9} Stream laeuft' : '\u{23FA} aufnahmebereit';
    $('vogel').textContent=s.vogelDrin
      ? '\u{1F423} Vogel ist drin ('+s.drinSeit+' s)' : 'Kasten leer';
    $('uhr').textContent=s.fps.toFixed(1)+' Bilder/s';

    // ---- Akku -------------------------------------------------------------
    const hatAkku=s.akkuVolt>0.5;
    $('ab').style.width=(hatAkku?s.akkuProzent:0)+'%';
    $('ab').style.background=s.akkuProzent<25?'#e0674f'
      :(s.akkuProzent<50?'var(--b)':'var(--a)');
    $('at').textContent=hatAkku
      ? s.akkuProzent+' % ('+s.akkuVolt.toFixed(2)+' V)' : 'kein Sensor';
    const pfeil={'1':'⬆ steigt — die Sonne l&auml;dt','0':'➡ stabil',
                 '-1':'⬇ f&auml;llt — l&auml;uft auf Akku'}[String(s.akkuTrend)];
    $('akkuinfo').innerHTML=hatAkku
      ? pfeil+' <span style="opacity:.6">(Verlauf &uuml;ber 10 Minuten)</span>'
      : 'Spannungssensor nicht angeschlossen — siehe Schaltplan 3.3.';
    $('akkupille').textContent=hatAkku
      ? '\u{1F50B} '+s.akkuProzent+' %'+(s.akkuTrend>0?' ⚡':'') : '\u{1F50B} —';

    // ---- Systemzustand ----------------------------------------------------
    const sy=s.system;
    const stufen=[['gut','✓ Alles in Ordnung'],['warn','⚠ Achtung'],
                  ['bad','✕ Problem']];
    $('ampel').className='ampel '+stufen[sy.gesundheit][0];
    $('ampel').textContent=stufen[sy.gesundheit][1];
    $('meldungen').innerHTML=sy.meldungen.map(m=>'<li>&middot; '+m+'</li>').join('');
    $('netzinfo').innerHTML=(sy.eigenesWlan
      ? '\u{1F4E1} <b>Eigenes WLAN</b> — die Kamera l&auml;uft autark, ohne Router.'
      : '\u{1F3E0} <b>Router-Betrieb</b> — die Kamera h&auml;ngt im Heimnetz.')
      +' Erreichbar unter <b>'+sy.ip+'</b>';

    // Netzwerk: im eigenen WLAN gibt es kein RSSI, dafuer Geraetezahl
    let q='keine Verbindung';
    if(sy.eigenesWlan){
      q=sy.clients+(sy.clients===1?' Gerät':' Geräte');
    }else if(sy.wlan){
      const r=sy.rssi;
      q=(r>-55?'sehr gut':r>-67?'gut':r>-75?'okay':r>-83?'schwach':'kritisch')
        +' ('+r+' dBm)';
    }
    $('s-wlan').textContent=q;
    $('s-wlan').nextElementSibling.textContent=
      sy.eigenesWlan?'verbunden mit Kamera':'WLAN-Signal';
    $('s-temp').textContent=sy.temperaturC.toFixed(0)+' °C';
    $('s-heap').textContent=sy.heapFreiKB+' KB';
    $('s-psram').textContent=(sy.psramFreiKB/1024).toFixed(1)+' MB';

    let sd='SD-Karte schreibt '+s.sdRate.toFixed(2)+' MB/s';
    if(s.verworfen>0){
      sd+=' &middot; <b style="color:var(--b)">'+s.verworfen+
          ' Bilder verworfen</b> &mdash; BILD_QUALITAET erhoehen!';
    }else if(s.sdRate>0){ sd+=' &middot; reicht aus ✓'; }
    $('sdinfo').innerHTML=sd;

    $('karteinfo').innerHTML='Karte '+sy.kartenBelegtMB+' von '+
      sy.kartenGroesseMB+' MB belegt ('+s.belegung+' %) &middot; '+
      s.clips+' Clips &middot; '+s.fotos+' Fotos';

    const std=Math.floor(sy.laufzeitS/3600), mn=Math.floor(sy.laufzeitS%3600/60);
    $('up').textContent=std+' h '+mn+' min';

    $('details').innerHTML=[
      ['Laufzeit', std+' h '+mn+' min'],
      ['Letzter Start', sy.neustartGrund],
      ['Betriebsart', sy.netzModus],
      ['Neuverbindungen', sy.wlanNeuverbindungen],
      ['Adresse', sy.ip],
      ['Uhrzeit gestellt', sy.zeitGesetzt?'ja':'NEIN'],
      ['Kamerafehler', sy.kameraFehler],
      ['Bilder pro Sekunde', s.fps.toFixed(1)],
      ['Arbeitsspeicher frei / min.', sy.heapFreiKB+' / '+sy.heapMinKB+' KB'],
      ['PSRAM frei / gesamt', sy.psramFreiKB+' / '+sy.psramGesamtKB+' KB'],
      ['Chip', sy.chip+' @ '+sy.takt+' MHz'],
      ['ESP-IDF', sy.sdkVersion],
      ['Lichtschranke', s.strahlFrei?'Strahl frei':'Strahl unterbrochen'],
      ['Durchfl&uuml;ge gez&auml;hlt', s.durchfluege],
      ['IR-Licht', s.nacht?'Nachtbetrieb':'aus (Tag)'],
      ['Mikrofon', s.tonLaeuft
        ?('l&auml;uft, '+s.tonRate+' Hz'+(s.tonImClip?', Clips mit Ton':'')) : 'aus'],
      ['Lautst&auml;rke jetzt', s.laut],
      ['Ton verworfen (Clip/Stream)', s.tonVerlorenClip+' / '+s.tonVerlorenStream],
      ['Im Kasten heute', Math.round(s.drinHeuteS/60)+' min'],
      ['Akku heute min / max', s.akkuMinHeute.toFixed(2)+' / '
        +s.akkuMaxHeute.toFixed(2)+' V'],
      ['Clips heute', s.clipsHeute]
    ].map(r=>'<tr><td>'+r[0]+'</td><td>'+r[1]+'</td></tr>').join('');

    stdHeute=s.stunden;
    rushhour(rhModus);

    // Beim ersten Durchlauf wissen wir erst jetzt, wie viele Tage die
    // Kamera zeigen soll (TAGE_ANZEIGEN aus config.h) — dann Verlauf holen.
    if(!s.tageAnzeigen){
      $('verlaufkarte').hidden=true; $('rh1').hidden=true;   // Archiv ist aus
    }else if(tageN!==s.tageAnzeigen){
      tageN=s.tageAnzeigen; $('tageN').textContent=tageN; tage();
    }
    $('tonbtn').hidden=!s.tonImStream;
  }catch(e){
    // Nicht still schlucken: wer die Seite debuggen will, soll den Fehler
    // in der Browser-Konsole (F12) sehen.
    console.error('Status konnte nicht aktualisiert werden:',e);
  }
}

// ---- Rushhour: heute oder Durchschnitt der letzten Tage -------------------
function rushhour(m){
  rhModus=m;
  $('rh0').classList.toggle('aktiv',m===0);
  $('rh1').classList.toggle('aktiv',m===1);
  const w=(m===0?stdHeute:(verlauf?verlauf.stundenSchnitt:null))||[];
  if(!w.length){$('balken').innerHTML='';return;}
  const max=Math.max(1,...w);
  const einheit=m===0?'':' im Schnitt';
  $('balken').innerHTML=w.map((v,i)=>
    `<div class="${v>0?'':'n'}" style="height:${Math.max(2,v/max*100)}%" title="${i}:00 Uhr: ${v}${einheit}"></div>`).join('');
  $('achse').innerHTML=w.map((v,i)=>`<span>${i%6?'':i}</span>`).join('');
}

// ---- Verlauf über Tage: kommt aus /tage.csv -------------------------------
async function tage(){
  try{
    const d=await(await fetch('/api/tage?n='+tageN)).json();
    verlauf=d;
    const t=d.tage;
    if(!t.length){
      $('tinfo').textContent='Noch keine Tageszeile in tage.csv — '
        +'die erste entsteht in der naechsten Stunde.';
      return;
    }
    const max=Math.max(1,...t.map(x=>x.besuche));
    $('tbalken').innerHTML=t.map(x=>{
      const tag=x.datum.slice(8)+'.'+x.datum.slice(5,7)+'.';
      return `<div class="${x.besuche?'':'n'}" style="height:${Math.max(2,x.besuche/max*100)}%"`
        +` title="${tag} ${x.besuche} Besuche, ${x.clips} Clips, `
        +`Akku ${x.akkuMin}-${x.akkuMax} V, ${Math.round(x.drinS/60)} min im Kasten"></div>`;
    }).join('');
    const jede=Math.ceil(t.length/6);
    $('tachse').innerHTML=t.map((x,i)=>
      `<span>${i%jede?'':x.datum.slice(8)+'.'+x.datum.slice(5,7)+'.'}</span>`).join('');

    const summe=t.reduce((a,x)=>a+x.besuche,0);
    const best=t.reduce((a,x)=>x.besuche>a.besuche?x:a,t[0]);
    const mitAkku=t.filter(x=>x.akkuMin>0.5);
    const tief=mitAkku.length
      ? mitAkku.reduce((a,x)=>x.akkuMin<a.akkuMin?x:a,mitAkku[0]) : null;
    $('tinfo').innerHTML='&Oslash; <b>'+(summe/t.length).toFixed(1)
      +'</b> Besuche am Tag &middot; bester Tag '
      +best.datum.slice(8)+'.'+best.datum.slice(5,7)+'. mit <b>'+best.besuche+'</b>'
      +(tief?' &middot; tiefster Akkustand <b>'+tief.akkuMin.toFixed(2)+' V</b> am '
        +tief.datum.slice(8)+'.'+tief.datum.slice(5,7)+'.':'')
      +' &middot; '+t.length+(t.length===1?' Tag':' Tage')+' im Archiv';
    if(rhModus===1) rushhour(1);
  }catch(e){
    console.error('Verlauf konnte nicht geladen werden:',e);
    $('tinfo').textContent='Verlauf konnte nicht geladen werden.';
  }
}

// ---- Ton an/aus ----------------------------------------------------------
// Browser dürfen Ton nicht von selbst starten (sonst würde jede Website
// losdudeln). Deshalb dieser Knopf: Ein Antippen ist die Erlaubnis.
function tonUm(){
  const a=$('ton');
  if(!tonAn){
    a.src='http://'+location.hostname+':82/';
    a.play().then(()=>{
      tonAn=true;
      $('tonbtn').textContent='\u{1F507} Ton aus';
      $('tonbtn').classList.add('aktiv');
      $('toninfo').textContent='Der Ton läuft dem Bild etwa eine Sekunde nach.';
    }).catch(e=>{
      $('toninfo').textContent='Der Browser will keinen Ton abspielen ('
        +e.message+'). Seite neu laden und nochmal tippen.';
    });
  }else{
    a.pause(); a.removeAttribute('src'); a.load();
    tonAn=false;
    $('tonbtn').textContent='\u{1F50A} Ton an';
    $('tonbtn').classList.remove('aktiv');
    $('toninfo').textContent='';
  }
}

async function lade(typ,btn){
  if(btn){document.querySelectorAll('nav button').forEach(b=>b.classList.remove('aktiv'));
    btn.classList.add('aktiv');}
  $('liste').innerHTML='<li style="color:var(--m);padding:9px">l&auml;dt&hellip;</li>';
  try{
    const d=await(await fetch('/api/liste?typ='+typ)).json();
    d.sort((a,b)=>b.zeit-a.zeit);
    $('liste').innerHTML=d.length?d.map(f=>
      `<li><a href="/datei?p=${encodeURIComponent(f.pfad)}">
        <span>${f.name}</span><small>${kb(f.groesse)} &middot; ${new Date(f.zeit*1000)
        .toLocaleString('de-DE',{day:'2-digit',month:'2-digit',hour:'2-digit',minute:'2-digit'})}</small>
      </a></li>`).join('')
      : '<li style="color:var(--m);padding:9px">Noch nichts aufgenommen.</li>';
  }catch(e){$('liste').innerHTML='<li style="padding:9px">Fehler beim Laden.</li>';}
}

status(); setInterval(status,5000); lade('clips');
// Der Verlauf ändert sich nur einmal pro Stunde — alle 10 Minuten reicht.
setInterval(tage,600000);
</script></body></html>)HTML";

// ---------------------------------------------------------------------------
//  Endpunkte
// ---------------------------------------------------------------------------
static void seiteAusliefern() {
  server.sendHeader("Cache-Control", "no-store");
  server.send_P(200, "text/html; charset=utf-8", SEITE);
}

static void statusAusliefern() {
  String j = statistikAlsJson();
  // die schließende Klammer ersetzen und eigene Werte anhängen
  j.remove(j.length() - 1);
  j += ",\"akkuVolt\":"    + String(akkuVolt(), 2);
  j += ",\"akkuProzent\":" + String(akkuProzent());
  j += ",\"nacht\":"       + String(istNacht ? "true" : "false");
  j += ",\"fps\":"         + String(aktuelleFps, 1);
  j += ",\"laufzeit\":"    + String(millis() / 1000);
  j += ",\"laut\":"        + String(audioLautstaerke());
  j += ",\"tonLaeuft\":"   + String(audioLaeuft() ? "true" : "false");
  j += ",\"tonRate\":"     + String(audioAbtastrate());
  j += ",\"tonImClip\":"   + String((TON_IN_CLIPS && audioLaeuft()) ? "true" : "false");
  j += ",\"tonImStream\":" + String((TON_IM_STREAM && audioLaeuft()) ? "true" : "false");
  j += ",\"tonVerlorenClip\":"   + String(tonVerloren(TON_ABNEHMER_CLIP));
  j += ",\"tonVerlorenStream\":" + String(tonVerloren(TON_ABNEHMER_STREAM));
  j += ",\"tageAnzeigen\":" + String(TAGE_CSV_AN ? TAGE_ANZEIGEN : 0);
  j += ",\"vogelDrin\":"   + String(vogelIstDrin() ? "true" : "false");
  j += ",\"drinSeit\":"    + String(vogelDrinSeitSekunden());
  j += ",\"durchfluege\":" + String(lichtschrankeDurchfluege());
  j += ",\"strahlFrei\":"  + String(strahlIstFrei() ? "true" : "false");
  j += ",\"zuschauer\":"   + String(webHatZuschauer() ? "true" : "false");
  j += ",\"verworfen\":"   + String(aviVerworfen());
  j += ",\"sdRate\":"      + String(aviSchreibrateMBs(), 2);
  j += ",\"akkuTrend\":"   + String(akkuTrend());
  j += ",\"system\":{"     + systemAlsJson() + "}";
  j += "}";
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", j);
}

static void listeAusliefern() {
  String typ = server.hasArg("typ") ? server.arg("typ") : "clips";
  if (typ != "clips" && typ != "fotos" && typ != "audio") typ = "clips";

  String ordner = "/" + typ;
  File dir = SD.open(ordner);
  if (!dir) { server.send(200, "application/json", "[]"); return; }

  String j = "[";
  bool erstes = true;
  File f;
  while ((f = dir.openNextFile())) {
    if (!f.isDirectory() && f.size() > 100) {   // leere Ringplätze überspringen
      if (!erstes) j += ",";
      erstes = false;
      String name = String(f.name());
      int schrag = name.lastIndexOf('/');
      if (schrag >= 0) name = name.substring(schrag + 1);
      j += "{\"name\":\"" + name + "\"";
      j += ",\"pfad\":\"" + ordner + "/" + name + "\"";
      j += ",\"groesse\":" + String((uint32_t)f.size());
      j += ",\"zeit\":" + String((uint32_t)f.getLastWrite()) + "}";
    }
    f.close();
  }
  dir.close();
  j += "]";
  server.send(200, "application/json", j);
}

static void dateiAusliefern() {
  if (!server.hasArg("p")) { server.send(400, "text/plain", "Pfad fehlt"); return; }
  String pfad = server.arg("p");

  // Sicherheitsprüfung: nur unsere drei Ordner, kein ".." nach oben
  if (pfad.indexOf("..") >= 0 ||
      !(pfad.startsWith("/clips/") || pfad.startsWith("/fotos/") || pfad.startsWith("/audio/"))) {
    server.send(403, "text/plain", "Nicht erlaubt");
    return;
  }

  File f = SD.open(pfad, FILE_READ);
  if (!f) { server.send(404, "text/plain", "Nicht gefunden"); return; }

  const char* typ = "application/octet-stream";
  if      (pfad.endsWith(".jpg")) typ = "image/jpeg";
  else if (pfad.endsWith(".wav")) typ = "audio/wav";
  else if (pfad.endsWith(".avi")) typ = "video/x-msvideo";

  server.streamFile(f, typ);
  f.close();
}

// ---------------------------------------------------------------------------
void webStart() {
  server.on("/",            seiteAusliefern);
  server.on("/api/status",  statusAusliefern);
  server.on("/api/liste",   listeAusliefern);
  // Nur die Systemdaten — praktisch zum Skripten oder für ein
  // Überwachungs-Dashboard im Haus.
  server.on("/api/system",  []() {
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", "{" + systemAlsJson() + "}");
  });

  // Uhrzeit stellen. Im eigenen WLAN gibt es kein Internet und damit keinen
  // Zeitserver — deshalb schenkt der erste Besucher der Kamera die Uhrzeit
  // seines Handys. Ohne das wären alle Zeitstempel falsch.
  server.on("/api/zeit", []() {
    if (!ZEIT_VOM_BROWSER) { server.send(403, "text/plain", "abgeschaltet"); return; }
    if (!server.hasArg("ts")) { server.send(400, "text/plain", "ts fehlt"); return; }

    // Nur stellen, wenn noch keine Zeit da ist — sonst würde jeder
    // Seitenaufruf die Uhr neu setzen und Zeitstempel könnten springen.
    if (zeitIstGesetzt()) { server.send(200, "text/plain", "schon gestellt"); return; }

    uint32_t ts = (uint32_t)strtoul(server.arg("ts").c_str(), nullptr, 10);
    bool ok = zeitSetzen(ts);
    server.send(ok ? 200 : 400, "text/plain", ok ? "gestellt" : "unplausibel");
  });
  server.on("/datei",       dateiAusliefern);

  // Die letzten Tage als JSON — daraus macht die Website den Verlauf.
  server.on("/api/tage", []() {
    uint16_t n = server.hasArg("n")
               ? (uint16_t)server.arg("n").toInt() : (uint16_t)TAGE_ANZEIGEN;
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", tageAlsJson(n));
  });

  // Die Rohdatei zum Herunterladen. Öffnet sich in Excel oder LibreOffice.
  server.on("/tage.csv", []() {
    // Vorher den laufenden Tag noch mitschreiben, sonst fehlt heute.
    tagJetztSichern();
    File f = SD.open(TAGE_DATEI, FILE_READ);
    if (!f) { server.send(404, "text/plain", "Noch keine Tagesstatistik."); return; }
    server.sendHeader("Content-Disposition", "attachment; filename=tage.csv");
    server.streamFile(f, "text/csv");
    f.close();
  });

  server.onNotFound([]() { server.send(404, "text/plain", "Gibt es nicht."); });

  server.begin();
  streamServer.begin();
  streamServer.setNoDelay(true);

  if (TON_IM_STREAM && audioLaeuft()) {
    tonServer.begin();
    tonServer.setNoDelay(true);
  }

  // netzwerkAdresse() liefert die richtige Adresse in BEIDEN Betriebsarten:
  // die Router-IP oder 192.168.4.1 im eigenen WLAN.
  String adr = netzwerkAdresse();
  Serial.printf("[Web] Website:    http://%s/\n", adr.c_str());
  Serial.printf("[Web] Livestream: http://%s:81/\n", adr.c_str());
  if (TON_IM_STREAM && audioLaeuft())
    Serial.printf("[Web] Tonstream:  http://%s:82/  (auf der Website \"Ton an\")\n",
                  adr.c_str());
  if (netzwerkIstEigenes())
    Serial.printf("[Web] (Handy zuerst mit dem WLAN \"%s\" verbinden!)\n", AP_NAME);
}

// ---------------------------------------------------------------------------
void webBedienen() {
  server.handleClient();

  // Neuer Zuschauer für den Livestream?
  if (streamServer.hasClient()) {
    if (streamClient && streamClient.connected()) {
      streamServer.available().stop();          // einer reicht
    } else {
      streamClient = streamServer.available();
      streamClient.print(
        "HTTP/1.1 200 OK\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Cache-Control: no-store\r\n"
        "Content-Type: multipart/x-mixed-replace; boundary=birdyframe\r\n"
        "\r\n");
      Serial.println("[Web] Zuschauer verbunden.");
    }
  }

  if (streamClient && !streamClient.connected()) {
    streamClient.stop();
    Serial.println("[Web] Zuschauer weg.");
  }

  tonBedienen();
}

bool webHatZuschauer() { return streamClient && streamClient.connected(); }

// ---------------------------------------------------------------------------
//  Der Tonstrom (Tür 82)
// ---------------------------------------------------------------------------
//  Wir schicken einen WAV-Kopf mit einer absurd großen Längenangabe und
//  danach einfach immer weiter Messwerte. Der Browser hört nicht auf zu
//  spielen, weil die Datei aus seiner Sicht nie zu Ende ist.
// ---------------------------------------------------------------------------
static void wavKopfSenden(WiFiClient& c, uint32_t rate) {
  uint8_t k[44];
  uint32_t riesig = 0x7FFFF000;          // "so lang wird die Datei" (~2 GB)
  memcpy(k + 0,  "RIFF", 4);
  *(uint32_t*)(k + 4)  = riesig + 36;
  memcpy(k + 8,  "WAVE", 4);
  memcpy(k + 12, "fmt ", 4);
  *(uint32_t*)(k + 16) = 16;
  *(uint16_t*)(k + 20) = 1;              // PCM
  *(uint16_t*)(k + 22) = 1;              // Mono
  *(uint32_t*)(k + 24) = rate;
  *(uint32_t*)(k + 28) = rate * 2;
  *(uint16_t*)(k + 32) = 2;
  *(uint16_t*)(k + 34) = 16;
  memcpy(k + 36, "data", 4);
  *(uint32_t*)(k + 40) = riesig;
  c.write(k, 44);
}

static void tonBedienen() {
  if (!TON_IM_STREAM || !audioLaeuft()) return;

  if (tonServer.hasClient()) {
    if (tonClient && tonClient.connected()) {
      tonServer.available().stop();          // einer reicht, wie beim Bild
    } else {
      tonClient = tonServer.available();
      tonClient.print(
        "HTTP/1.1 200 OK\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Cache-Control: no-store\r\n"
        "Content-Type: audio/wav\r\n"
        "Connection: close\r\n"
        "\r\n");
      wavKopfSenden(tonClient, audioAbtastrate());
      tonAufsetzen(TON_ABNEHMER_STREAM);    // ab jetzt, nicht die alte Konserve
      Serial.println("[Web] Zuhoerer verbunden.");
    }
  }

  if (tonClient && !tonClient.connected()) {
    tonClient.stop();
    Serial.println("[Web] Zuhoerer weg.");
    return;
  }
  if (!tonClient) return;

  // Nur so viel schicken, wie in den Sendepuffer passt. Sonst würde write()
  // warten, bis das WLAN wieder Luft hat — und die Kamera würde stehen.
  int platz = tonClient.availableForWrite();
  if (platz < 256) return;
  size_t maxProben = min((size_t)(platz / 2), (size_t)(sizeof(tonAus) / 2));
  size_t n = tonAbholen(TON_ABNEHMER_STREAM, tonAus, maxProben);
  if (n) tonClient.write((const uint8_t*)tonAus, n * sizeof(int16_t));
}

// ---------------------------------------------------------------------------
//  Ein Bild in den Livestream schieben.
//  Das Format heißt MJPEG: einfach ein JPEG nach dem anderen, jeweils mit
//  einer Trennzeile dazwischen. Jeder Browser kann das von sich aus.
// ---------------------------------------------------------------------------
void webFrameSenden(camera_fb_t* bild) {
  if (!bild || !webHatZuschauer()) return;

  char kopf[96];
  int n = snprintf(kopf, sizeof(kopf),
                   "--birdyframe\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
                   (unsigned)bild->len);
  streamClient.write((const uint8_t*)kopf, n);
  streamClient.write(bild->buf, bild->len);
  streamClient.write((const uint8_t*)"\r\n", 2);
}
