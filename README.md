# 🔐 ESP32 WiFi Security Trainer

![Version](https://img.shields.io/badge/version-3.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)
![License](https://img.shields.io/badge/license-Educational-orange.svg)

একটি সম্পূর্ণ **শিক্ষামূলক WiFi Security Training টুল** যা ESP32 দিয়ে তৈরি। এটি WiFi নিরাপত্তা সম্পর্কে জানতে এবং শিখতে ব্যবহার করা যায়।

---

## 📋 সূচিপত্র

- [ফিচার সমূহ](#-ফিচার-সমূহ)
- [প্রয়োজনীয় উপকরণ](#-প্রয়োজনীয়-উপকরণ)
- [ইনস্টলেশন](#-ইনস্টলেশন)
- [কিভাবে ব্যবহার করবেন](#-কিভাবে-ব্যবহার-করবেন)
- [ফিচার বিস্তারিত](#-ফিচার-বিস্তারিত)
- [সতর্কতা](#-সতর্কতা)
- [FAQ](#-faq)
- [লাইসেন্স](#-লাইসেন্স)

---

## 🚀 ফিচার সমূহ

### ✅ 1. WiFi Network Scanner
- **দ্রুত স্ক্যানিং:** 200ms timeout এ fast scanning
- **Signal Strength Indicator:** 4-bar visual indicator
- **বিস্তারিত তথ্য:** SSID, BSSID, Channel, RSSI
- **Real-time Update:** প্রতি 30 সেকেন্ডে auto-scan

### ⚡ 2. Deauth Attack (Educational)
- **আসল 802.11 Deauth Packets** পাঠানো হয়
- **500 packets/sec** speed (প্রতি 10ms এ 5টি packet)
- **Broadcast deauth** সব clients-এ কাজ করে
- **Promiscuous mode** সঠিক channel switching
- **Real-time statistics** packet count tracking

### 👥 3. Evil Twin AP
- **Dual Operation Mode:**
  - Admin Access: `http://192.168.4.1/admin`
  - Victim Portal: `http://192.168.4.1`
- **Captive Portal Detection** (Android/iOS)
- **Credential Harvesting** username, password capture
- **IP Tracking** client IP logging
- **Admin Connection Maintained** - Evil Twin চালু থাকলেও Admin disconnect হয় না

### 🎨 4. Professional Web Interface
- **100% Responsive Design** - মোবাইল ও ডেস্কটপ বান্ধব
- **Modern UI/UX** - Gradient colors, animations
- **Live Statistics** - Real-time data updates
- **Network Selection** - Visual feedback with highlighting
- **Dark Mode Ready** - Eye-friendly color scheme

---

## 🛠 প্রয়োজনীয় উপকরণ

### Hardware:
- **ESP32 Development Board** (যেকোনো variant)
- **Micro USB Cable**
- **Computer** (Windows/Mac/Linux)

### Software:
- **VS Code** (Latest version)
- **PlatformIO Extension**
- **Git** (Optional)

---

## 📦 ইনস্টলেশন

### Step 1: VS Code & PlatformIO Setup

```bash
# VS Code Download
https://code.visualstudio.com/

# PlatformIO Extension Install
1. VS Code খুলুন
2. Extensions (Ctrl+Shift+X) এ যান
3. "PlatformIO IDE" খুঁজুন
4. Install করুন
```

### Step 2: Project Clone/Download

```bash
# Git দিয়ে Clone করুন
git clone <your-repo-url>
cd wifi_security_trainer

# অথবা ZIP Download করে Extract করুন
```

### Step 3: Configuration

**`src/main.cpp`** ফাইল খুলে Admin credentials পরিবর্তন করুন:

```cpp
const char* adminSSID = "YourSSIDName";  // আপনার পছন্দের SSID
const char* adminPASS = "YourPassword";   // শক্তিশালী password দিন
```

### Step 4: Build & Upload

```bash
# PlatformIO Terminal এ:
pio run                    # Build
pio run --target upload    # Upload to ESP32

# অথবা VS Code UI থেকে:
# 1. PlatformIO icon ক্লিক করুন
# 2. Project Tasks > Build
# 3. Project Tasks > Upload
```

### Step 5: Serial Monitor (Optional)

```bash
pio device monitor -b 115200

# VS Code UI:
# PlatformIO > Project Tasks > Monitor
```

---

## 🎯 কিভাবে ব্যবহার করবেন

### প্রথম চালু করা

1. **ESP32 Power On করুন**
   ```
   ✅ Device চালু হবে
   📡 Admin AP তৈরি হবে
   🌐 Web Server শুরু হবে
   ```

2. **WiFi Connect করুন**
   ```
   SSID: Unknown (অথবা আপনার দেওয়া নাম)
   Password: R@bbi0606@ (অথবা আপনার দেওয়া password)
   ```

3. **Browser Open করুন**
   ```
   URL: http://192.168.4.1
   ```

### Admin Dashboard

#### 📶 Network Scanning
```
1. "🔍 Scan Networks" বাটনে ক্লিক করুন
2. Loading animation দেখাবে
3. সব available networks দেখাবে
4. যে network চান সেটাতে ক্লিক করুন (হাইলাইট হবে)
```

#### ⚡ Deauth Attack
```
1. একটি network select করুন
2. "⚡ Start Deauth" বাটনে ক্লিক করুন
3. Attack চলবে এবং packet count বাড়বে
4. "⏹️ Stop Deauth" চাপলে বন্ধ হবে
```

#### 👥 Evil Twin AP
```
1. একটি network select করুন
2. Settings tab-এ Evil Twin Password সেট করুন (ঐচ্ছিক)
3. "👥 Create Evil Twin" বাটনে ক্লিক করুন
4. SSID পরিবর্তন হবে target network এর মতো
```

**⚠️ Evil Twin Active থাকলে:**
- **Admin Access:** `http://192.168.4.1/admin` (বুকমার্ক করুন!)
- **Victim Access:** `http://192.168.4.1` (auto-redirect to login page)

#### 🔑 Credentials View
```
1. "Credentials" tab-এ যান
2. Captured username/password দেখুন
3. Password cell-এ ক্লিক করলে reveal হবে
4. Serial Monitor-এও দেখাবে
```

---

## 📚 ফিচার বিস্তারিত

### 1. Network Scanner Architecture

```cpp
WiFi.mode(WIFI_AP_STA);                      // AP + Station mode
int n = WiFi.scanNetworks(false, false, false, 200);  // Fast scan
```

**Features:**
- Non-blocking scan
- AP connection maintained
- 200ms timeout for speed
- Auto cleanup with `WiFi.scanDelete()`

### 2. Deauth Attack Mechanism

```cpp
// 802.11 Deauth Frame Structure
uint8_t deauthPacket[26] = {
    0xC0, 0x00,                         // Type: Deauthentication
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: Broadcast
    // ... BSSID and other fields
    0x07, 0x00                          // Reason: Class 3 frame
};
```

**কিভাবে কাজ করে:**
1. Target BSSID parse করা হয়
2. Promiscuous mode enable করা হয়
3. সঠিক channel-এ switch করা হয়
4. প্রতি 10ms এ 5টি packet পাঠানো হয়
5. Broadcast address-এ পাঠানো হয় (সব clients disconnect)

**Technical Details:**
- **Packet Rate:** 500 packets/second
- **Frame Type:** Management Frame (0xC0)
- **Reason Code:** 0x07 (Class 3 frame received from non-associated STA)

### 3. Evil Twin + Admin Dual Mode

**Architecture:**
```
┌─────────────────────────────────┐
│     ESP32 Soft AP (Port 80)     │
├─────────────────────────────────┤
│  Route: /        → Smart Router │
│  Route: /admin   → Admin Panel  │
│  Route: /scan    → API Endpoint │
└─────────────────────────────────┘
         │
         ├─► Victim Request → Phishing Page
         └─► Admin Request  → Dashboard
```

**Smart Routing Logic:**
```cpp
if(isEvilTwinActive) {
    if(request.hasArg("admin") || request.uri() == "/admin") {
        // Show Admin Dashboard
    } else {
        // Show Phishing Page
    }
}
```

### 4. Captive Portal Detection

**Android Detection:**
```cpp
phishingServer.on("/generate_204", HTTP_GET, []() {
    phishingServer.send(200, "text/html", phishingPage);
});
```

**iOS Detection:**
```cpp
phishingServer.on("/hotspot-detect.html", HTTP_GET, []() {
    phishingServer.send(200, "text/html", phishingPage);
});
```

### 5. Credential Capture System

```cpp
struct Credential {
    String timestamp;    // HH:MM:SS format
    String ssid;        // Network name
    String username;    // Optional field
    String password;    // Captured password
    String clientIP;    // Client IP address
};
```

**Storage:**
- Array of 100 credentials
- Saved in RAM (lost on restart)
- Real-time Serial output
- Web dashboard display

---

## 🔧 Configuration Options

### platformio.ini

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps = 
    bblanchon/ArduinoJson@^6.21.3
```

### Custom Settings

**Change Admin Credentials:**
```cpp
const char* adminSSID = "MySecureAP";
const char* adminPASS = "MyStr0ngP@ss123";
```

**Change Evil Twin Password:**
```cpp
String evilTwinPASS = "connect123";  // Default password
```

**Adjust Deauth Speed:**
```cpp
// In loop() function:
if(isDeauthActive && millis() - lastDeauth > 10) {  // Change 10 to higher value
```

**Max Clients:**
```cpp
WiFi.softAP(ssid.c_str(), password.c_str(), channel, 0, 8);  // Change 8 to desired value
```

---

## ⚠️ সতর্কতা

### ⛔ আইনগত সতর্কতা

```
⚠️ গুরুত্বপূর্ণ নোটিশ:

এই টুল শুধুমাত্র শিক্ষামূলক উদ্দেশ্যে তৈরি করা হয়েছে।
```

**নিষিদ্ধ কাজ:**
- ❌ অনুমতি ছাড়া অন্যের নেটওয়ার্কে attack করা
- ❌ Public WiFi এ unauthorized testing
- ❌ ক্ষতিকর উদ্দেশ্যে ব্যবহার করা

**অনুমোদিত ব্যবহার:**
- ✅ নিজের নেটওয়ার্কে testing
- ✅ Controlled lab environment
- ✅ Authorized penetration testing
- ✅ Educational demonstrations

**আইনি দায়বদ্ধতা:**
> ব্যবহারকারী নিজে সম্পূর্ণ দায়বদ্ধ। ডেভেলপার কোনো অবৈধ ব্যবহারের জন্য দায়ী নয়।

### 🛡️ নিরাপত্তা টিপস

1. **Admin Password পরিবর্তন করুন**
   ```cpp
   // ডিফল্ট password ব্যবহার করবেন না!
   const char* adminPASS = "YourStrongPassword123!";
   ```

2. **Evil Twin Password শক্তিশালী রাখুন**
   ```
   ❌ Bad: 123456, password, admin
   ✅ Good: C0mpl3x!P@ssw0rd
   ```

3. **Testing Environment**
   - Isolated network ব্যবহার করুন
   - Real users প্রভাবিত হবে না এমন স্থানে test করুন

4. **Data Protection**
   - Captured credentials নিরাপদে রাখুন
   - Testing শেষে credentials delete করুন

---

## 📱 Mobile App Access

### Android Setup
```
1. WiFi-তে connect করুন
2. Auto-redirect হবে (Captive Portal)
3. অথবা manually: http://192.168.4.1 visit করুন
```

### iOS Setup
```
1. WiFi-তে connect করুন
2. "Sign in to network" notification আসবে
3. Tap করুন অথবা Safari-এ http://192.168.4.1
```

### Desktop Browser
```
Recommended: Chrome, Firefox, Edge
URL: http://192.168.4.1
Admin URL: http://192.168.4.1/admin
```

---

## 🔍 FAQ

### Q1: ESP32 boot হচ্ছে না / upload হচ্ছে না?

**Solution:**
```bash
# BOOT button ধরে রেখে Upload button চাপুন
# USB cable/port পরিবর্তন করুন
# Driver install করুন: CP2102 or CH340
```

### Q2: WiFi AP দেখা যাচ্ছে না?

**Check:**
```cpp
// Serial Monitor দেখুন
// SSID name confirm করুন
// 2.4GHz WiFi enabled আছে কিনা (ESP32 শুধু 2.4GHz)
```

### Q3: Admin panel access করতে পারছি না (Evil Twin active)?

**Solution:**
```
URL: http://192.168.4.1/admin (must use /admin)
Browser bookmark করুন
Clear browser cache
```

### Q4: Deauth attack কাজ করছে না?

**Reasons:**
```
✓ Target device-এ WPA3 থাকতে পারে (deauth protected)
✓ Router-এ Management Frame Protection enabled
✓ Modern devices reconnect দ্রুত
✓ Channel ঠিক আছে কিনা check করুন
```

### Q5: Credentials capture হচ্ছে না?

**Check:**
```
1. Evil Twin active আছে কিনা
2. Port 8080 server চলছে কিনা (Serial Monitor দেখুন)
3. Victim correct URL-এ যাচ্ছে কিনা (192.168.4.1)
4. Form submit হচ্ছে কিনা (Serial Monitor)
```

### Q6: Device hang/restart হচ্ছে?

**Solutions:**
```cpp
// Power supply check করুন (minimum 500mA)
// USB hub ব্যবহার না করে direct connect করুন
// Deauth speed কমান:
if(isDeauthActive && millis() - lastDeauth > 50) {  // 10 থেকে 50
```

### Q7: Memory error দেখাচ্ছে?

**Optimization:**
```cpp
// Credential array size কমান:
Credential capturedCreds[50];  // 100 থেকে 50

// Network array size কমান:
String networkSSIDs[25];  // 50 থেকে 25
```

---

## 📊 Performance Metrics

### System Specifications

```yaml
Platform: ESP32 (Dual Core 240MHz)
RAM Usage: ~180KB / 320KB
Flash Usage: ~850KB / 4MB
WiFi: 802.11 b/g/n (2.4GHz)
Max Clients: 8 simultaneous
Web Server: 80 (Admin) + 8080 (Evil Twin)
```

### Speed Benchmarks

```
Network Scan: ~2-3 seconds (200ms timeout)
Deauth Rate: 500 packets/second
Web Response: <100ms (local network)
Credential Capture: Real-time (instant)
```

### Power Consumption

```
Idle Mode: ~80mA @ 5V
Active Scan: ~120mA @ 5V
Deauth Attack: ~150mA @ 5V
Max Load: ~180mA @ 5V
```

---

## 🎓 শিক্ষামূলক উদ্দেশ্য

এই প্রজেক্ট দিয়ে শিখুন:

### 1. WiFi Security Concepts
- WPA/WPA2 authentication
- Deauthentication attacks
- Evil Twin AP technique
- Captive portal mechanism

### 2. ESP32 Programming
- WiFi library usage
- Web server implementation
- Dual mode operation (AP + STA)
- Promiscuous mode packet injection

### 3. Web Development
- Responsive HTML/CSS
- JavaScript fetch API
- Real-time updates
- Mobile-first design

### 4. Network Protocols
- 802.11 management frames
- DHCP server operation
- DNS redirection
- HTTP request handling

---

## 🔄 Version History

### v3.0 (Current) - February 2026
```
✨ New Features:
- Professional responsive UI
- Signal strength indicators
- Dual server mode (Admin + Evil Twin)
- Real deauth attack implementation
- Captive portal support
- Mobile-friendly design

🐛 Bug Fixes:
- Fixed network disconnect issue during scan
- Admin connection maintained in Evil Twin mode
- Forward function declarations added
- Memory optimization

⚡ Performance:
- Fast scanning (200ms timeout)
- 500 packets/sec deauth rate
- Optimized packet sending
```

### v2.0 - Initial Release
```
- Basic WiFi scanner
- Simulated deauth
- Simple Evil Twin
- Basic web interface
```

---

## 🤝 Contributing

Contributions welcome! এই প্রজেক্ট improve করতে চান?

```bash
# Fork করুন
# Branch তৈরি করুন
git checkout -b feature/AmazingFeature

# Commit করুন
git commit -m 'Add some AmazingFeature'

# Push করুন
git push origin feature/AmazingFeature

# Pull Request খুলুন
```

---

## 📞 Support & Contact

### Issues/Bugs Report করুন:
```
GitHub Issues: [Your repo URL]/issues
```

### Documentation:
```
README: এই ফাইল
Code Comments: src/main.cpp
```

### Community:
```
Discussions: [Your repo URL]/discussions
```

---

## 📜 লাইসেন্স

```
MIT License (Educational Purpose Only)

Copyright (c) 2026 ESP32 WiFi Security Trainer

This tool is for EDUCATIONAL PURPOSES ONLY.
Any misuse or illegal activity is strictly prohibited.
Users are solely responsible for their actions.
```

---

## 🙏 Acknowledgments

**Libraries Used:**
- Arduino Core for ESP32
- ESP32 WiFi Library
- ArduinoJson by Benoit Blanchon
- WebServer Library

**Inspired By:**
- WiFi security research community
- Ethical hacking education
- Open source penetration testing tools

---

## 📸 Screenshots

### Admin Dashboard
```
┌────────────────────────────────────┐
│   🔐 Cyber Security Trainer        │
│   ──────────────────────────       │
│                                    │
│   📶 Network Scanner  | 🎯 Controls│
│   ├─ Network 1        | Target: -  │
│   ├─ Network 2        | Packets: 0 │
│   └─ Network 3        | Creds: 0   │
│                       |            │
│   [Scan] [Deauth] [Evil Twin]     │
└────────────────────────────────────┘
```

### Evil Twin Login Page
```
┌────────────────────────────────────┐
│           🔐                        │
│   WiFi Authentication Required     │
│                                    │
│   Network: [Target SSID]           │
│                                    │
│   Username: [________]             │
│   Password: [________]             │
│                                    │
│   [Connect to WiFi]                │
│                                    │
│   ⚠️ Educational Demo Only         │
└────────────────────────────────────┘
```

---

## 🚀 Quick Start Checklist

- [ ] VS Code installed
- [ ] PlatformIO extension installed
- [ ] ESP32 board connected
- [ ] Admin credentials changed
- [ ] Project built successfully
- [ ] Uploaded to ESP32
- [ ] Serial monitor working
- [ ] Admin AP visible
- [ ] Web interface accessible
- [ ] All features tested

---

## 📝 Notes

**Important Reminders:**
1. ✅ Always test in isolated environment
2. ✅ Get proper authorization before testing
3. ✅ Keep firmware updated
4. ✅ Backup your configuration
5. ✅ Document your findings
6. ✅ Practice responsible disclosure
7. ✅ Use for learning purposes only

**Best Practices:**
- Regular code reviews
- Security-first approach
- Clear documentation
- Version control usage
- Community collaboration

---

<div align="center">

### Made with ❤️ for Cybersecurity Education

**⭐ Star this repo if you found it helpful!**

```
┌─────────────────────────────────────────┐
│  Remember: With great power comes      │
│  great responsibility. Use wisely! 🛡️  │
└─────────────────────────────────────────┘
```

</div>

---

**Last Updated:** February 2026  
**Version:** 3.0  
**Status:** Active Development

