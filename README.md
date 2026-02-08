# 🔐 ESP32 Evil Twin Trainer

![Version](https://img.shields.io/badge/version-4.3-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)
![License](https://img.shields.io/badge/license-Educational-orange.svg)

**Optimized Evil Twin WiFi Security Training Tool** - Evil Twin only features for ESP32

---

## 🎯 Overview

Fully optimized Evil Twin attack tool with:

✅ **Features:**
- 👥 Evil Twin AP (Fake WiFi)
- 🎯 Captive Portal (Auto Login Page)
- 🔐 Credential Capture
- 📶 WiFi Network Scanner
- 🎨 Minimal Terminal UI (Spacehuhn Style)
- 🌐 DNS Server (Captive Portal)
- ⚙️ Admin Settings (Configurable SSID/Password/Hide)
- 📱 100% Realistic Android WiFi Dialog
- 🔄 Auto-Connect & Password Validation

🚀 **Benefits:**
- 70% less code (1839 → 550 lines)
- 60% less memory (120KB → 50KB RAM)
- Fast response (<200ms page load)
- Minimal terminal UI
- Better stability
- Fully customizable
- 100% realistic Android dialog
- Smart password validation

---

## 🚀 Key Features

### 🔄 Auto-Connect & Password Validation (v4.3)
Background password testing against original network. If correct, automatically connects user to real network and stops Evil Twin. If wrong, attack continues.

### 📱 Android WiFi Dialog (v4.2)
100% realistic Android WiFi dialog design with Material UI, dark theme, working password toggle, and mobile optimization.

### ⚙️ Admin Settings (v4.1)
Configure SSID, password, and hide/show options from web dashboard. Settings stored in flash memory, persistent across restarts.

### 👥 Evil Twin AP
Open network (no password) with captive portal, DNS hijacking, instant credential capture, and multi-device support.

📄 **Complete Features Documentation:** [FEATURES.md](FEATURES.md)  
⚡ **Quick Start Guide:** [QUICK_GUIDE.md](QUICK_GUIDE.md)

---

## 📦 Installation

### Requirements
- ESP32 Board
- Micro USB Cable
- VS Code + PlatformIO

### Quick Setup

```bash
# 1. Open project
code d:\ESP-32\wifi_security_trainer

# 2. Build & Upload
pio run -t upload

# 3. Monitor
pio device monitor
```

---

## 🎯 Usage

### Step 1: Power On ESP32
```
✅ Device boots
📡 Admin AP: "Unknown"
🌐 IP: 192.168.4.1
```

### Step 2: Connect
```
WiFi SSID: Unknown
Password: Rabbi0606@
Browser: http://192.168.4.1
```

### Step 3: Create Evil Twin
1. Click "SCAN"
2. Select target network
3. Click "START"
4. ✅ Evil Twin active!

### Step 4: Admin Access (Important!)
```
📱 Admin Panel: http://192.168.4.1/admin
🎯 Victims see: http://192.168.4.1 (Login page)
```

⚠️ **Important:** Bookmark `/admin` URL before starting Evil Twin!

### Step 5: View Credentials
- Dashboard shows real-time credential table
- Serial Monitor shows instant logs
- Data: Timestamp, SSID, Password, Client IP

### Step 6: Stop Attack
Click "STOP" button to restore everything.

---

## 📊 Technical Details

### Memory Usage

| Component | Before (v3.0) | Now (v4.3) | Savings |
|-----------|---------------|------------|---------|
| Networks | 50 items | 30 items | 40% |
| Credentials | 100 items | 50 items | 50% |
| Deauth Code | ✅ | ❌ | 30KB |
| HTML/CSS | 1850 lines | 400 lines | 78% |
| **Total RAM** | **~120KB** | **~50KB** | **58%** |

### Performance
```
✅ Scan Speed: 300ms
✅ Page Load: <200ms
✅ DNS Response: <10ms
✅ Max Clients: 8
✅ Credential Capture: Instant
✅ Password Validation: <10s
```

### Code Size
```
Before: 1839 lines
Now: 550 lines
Savings: 70%
```

---

## 🔧 Configuration

### WiFi Power
```cpp
WiFi.setTxPower(WIFI_POWER_19_5dBm);  // Maximum (default)
WiFi.setTxPower(WIFI_POWER_15dBm);    // Medium (battery save)
```

### Storage Limits
```cpp
networkCount = min(n, 30);        // Increase to 50 if needed
Credential capturedCreds[50];     // Increase to 100 if needed
```

---

## 📱 Captive Portal Support

All major platforms supported:

| Platform | Endpoint | Status |
|----------|----------|--------|
| Android | `/generate_204` | ✅ |
| iOS/macOS | `/hotspot-detect.html` | ✅ |
| Windows | `/connecttest.txt` | ✅ |
| Linux | `/` | ✅ |

---

## 🐛 Troubleshooting

### Upload Failed
```bash
# Check COM port
# Hold BOOT button during upload
```

### Captive Portal Not Working
```
✓ Check DNS server in Serial Monitor
✓ Verify open network (no password)
✓ Clear device cache
```

### Evil Twin Not Visible
```
✓ Check WiFi power (19.5dBm)
✓ Try different channel
✓ Move ESP32 closer
```

---

## ⚠️ Legal Warning

### Educational Purposes Only!

⚠️ **This tool is for educational purposes only.**

**Legal Notice:**
- ✅ Test on your own networks only
- ❌ Do not use on others' networks without permission
- ✅ For WiFi security awareness
- ❌ Not for illegal purposes

**Ethical Guidelines:**
- Obtain permission before testing
- Keep captured data secure
- Practice responsible disclosure
- Security research only

---

## 📚 How It Works

### Evil Twin Attack Flow

```
1️⃣ Real Network Scan
   └─> "Starbucks_WiFi" found (Channel 6)

2️⃣ Evil Twin Create
   └─> "Starbucks_WiFi" clone (Ch 6, Open)
   └─> Stronger signal = Victims connect

3️⃣ DNS Hijacking
   └─> All domain requests → 192.168.4.1
   └─> google.com → Login Page

4️⃣ Captive Portal
   └─> Android/iOS auto-detect
   └─> "Sign in to network" notification

5️⃣ Credential Capture
   └─> Victim enters password
   └─> Instantly saved

6️⃣ Auto-Validation (v4.3)
   └─> Tests password on real network
   └─> Success → Auto-stop & reconnect
   └─> Failure → Continue attack
```

---

## 💡 FAQ

**Q: Why no Deauth attack?**
> Evil Twin alone is effective enough. Deauth uses more memory and isn't needed in most cases.

**Q: How many victims can it handle?**
> 8 clients simultaneously. Can be increased if needed.

**Q: Does it work on all devices?**
> Yes, supports Android, iOS, Windows, Mac, Linux.

**Q: What's the range?**
> 50-100 meters (open space) at 19.5dBm power. Less indoors.

**Q: Where are passwords saved?**
> ESP32 RAM. Cleared on restart. Use SPIFFS/SD card for permanent storage.

---

## 🔄 Version History

### v4.3 (Current)
- ✅ Auto-connect & password validation
- ✅ Smart attack logic
- ✅ Seamless user experience

### v4.2
- ✅ 100% realistic Android WiFi dialog
- ✅ Material Design UI
- ✅ Mobile optimized

### v4.1
- ✅ Configurable admin settings
- ✅ Persistent storage (Flash)
- ✅ Hide/show SSID option

### v4.0
- ✅ Evil Twin only (optimized)
- ✅ 70% code reduction
- ✅ Minimal terminal UI

---

## 📞 Support

**Issues:** GitHub Issues  
**Questions:** Discussions  
**Updates:** Watch repository

---

## 📄 License

Educational Purposes Only.  
Check license before commercial use.

---

## 🎯 Learning Outcomes

From this project you'll learn:

### WiFi Security
- Evil Twin mechanism
- Captive portal techniques
- DNS hijacking
- Credential harvesting
- SSID spoofing

### ESP32 Programming
- WiFi AP configuration
- Web server setup
- DNS server implementation
- Memory optimization
- JSON handling

### Web Development
- Responsive design
- Form handling
- AJAX
- Captive portal UI
- Mobile optimization

---

## 📚 Documentation

- **[FEATURES.md](FEATURES.md)** - Complete features documentation
- **[QUICK_GUIDE.md](QUICK_GUIDE.md)** - Quick start guide (5 minutes)

---

**Made with ❤️ for WiFi Security Education**

*Remember: Use responsibly. Knowledge is power, use it wisely!*

