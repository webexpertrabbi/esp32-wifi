# ⚡ ESP32 Evil Twin - Quick Start Guide

## 🚀 দ্রুত শুরু (5 মিনিটে)

### ধাপ 1: Upload করুন
```bash
# VS Code এ PlatformIO > Upload (Ctrl+Alt+U)
# অথবা Terminal থেকে:
pio run -t upload
```

### ধাপ 2: Connect করুন
```
WiFi: Unknown
Password: Rabbi0606@
Browser: http://192.168.4.1
```

### ধাপ 3: Evil Twin তৈরি করুন
```
1. "SCAN" ক্লিক করুন
2. Target network select করুন
3. "START" ক্লিক করুন
4. ✅ Evil Twin active!
```

### ধাপ 4: Credentials দেখুন
```
Admin Panel: http://192.168.4.1/admin
Serial Monitor: 115200 baud
Table এ captured passwords দেখুন
```

---

## 🔑 Important Info

### Default Credentials
```
Admin SSID: Unknown
Admin Password: Rabbi0606@
Admin IP: 192.168.4.1
```

### URLs

| Purpose | URL |
|---------|-----|
| Admin Panel | http://192.168.4.1/admin |
| Victim Portal | http://192.168.4.1 |

⚠️ **গুরুত্বপূর্ণ:** Evil Twin active করার আগে `/admin` বুকমার্ক করুন!

---

## ⚙️ Settings পরিবর্তন করতে

### Admin Settings
```
1. http://192.168.4.1/admin → SETTINGS
2. SSID/Password পরিবর্তন করুন
3. "SAVE SETTINGS" ক্লিক করুন
4. ESP32 restart করুন
5. নতুন credentials দিয়ে connect করুন
```

### Reset to Default
```
Dashboard → "RESET TO DEFAULT" → Restart
অথবা: pio run -t erase → pio run -t upload
```

---

## 📱 কিভাবে কাজ করে

### User Journey
```
1. User Evil Twin network দেখে (Open WiFi)
2. Connect করে (no password needed)
3. Captive portal notification আসে
4. Android WiFi dialog খোলে (100% realistic!)
5. Password enter করে + "Connect" click করে
6. ESP32 background এ password test করে:
   
   ✅ Correct → User real network এ connect হয়
              → Internet works!
              → Evil Twin auto-stops
   
   ❌ Wrong → Evil Twin continues
            → User can try again
```

### Admin View
```
Serial Monitor এ instant feedback:
- [CREDENTIAL] Password captured
- [AUTO-CONNECT] Testing password...
- [AUTO-CONNECT] ✅ CORRECT! or ❌ WRONG
- Dashboard এ table update হয়
```

---

## 🔧 Configuration

### WiFi Power
```cpp
// src/main.cpp এ:
WiFi.setTxPower(WIFI_POWER_19_5dBm);  // Maximum (default)
WiFi.setTxPower(WIFI_POWER_15dBm);    // Medium
```

### Storage Limits
```cpp
String networkSSIDs[30];        // 30 networks (change to 50 if needed)
Credential capturedCreds[50];   // 50 credentials (change to 100)
```

### Connection Timeout
```cpp
int attempts = 20;  // 10 seconds (20 × 500ms)
// Fast: 10 attempts (5 seconds)
// Slow: 40 attempts (20 seconds)
```

---

## 📊 Status LEDs (Serial Monitor)

### Boot Messages
```
✅ ADMIN ACCESS POINT CREATED
   SSID: Unknown
   Password: Rabbi0606@
   IP: 192.168.4.1

✅ WEB SERVER STARTED
✅ SYSTEM READY
```

### Evil Twin Active
```
[EVIL TWIN] ✅ Created successfully!
  SSID: TargetNetwork (Open - No Password)
  Channel: 6
  IP: 192.168.4.1
  📱 Victims: http://192.168.4.1
  🔐 Admin: http://192.168.4.1/admin
```

### Password Captured
```
[CREDENTIAL] 📧 Captured!
  Time: 00:05:23
  SSID: TargetNetwork
  Password: UserPassword123
  Client IP: 192.168.4.2

[AUTO-CONNECT] Attempting to connect...
[AUTO-CONNECT] ✅ Password is CORRECT!
[AUTO-STOP] ✅ Evil Twin stopped
```

---

## 🐛 Common Issues

### 1. Upload Failed
```
Problem: Can't upload to ESP32
Solution:
- Check COM port
- Hold BOOT button during upload
- Close Serial Monitor
- Try different USB cable
```

### 2. Evil Twin দেখা যাচ্ছে না
```
Problem: Created but not visible
Solution:
- Check WiFi power (should be 19.5dBm)
- Try different channel
- Move ESP32 closer
- Restart ESP32
```

### 3. Captive Portal খুলছে না
```
Problem: No login page appears
Solution:
- DNS server running? Check Serial Monitor
- Network is Open (no password)?
- Clear device WiFi cache
- Reconnect to Evil Twin
```

### 4. Password Validation ব্যর্থ
```
Problem: Always shows "INCORRECT"
Solution:
- Original WiFi in range?
- WPA2 network? (WPA3 not supported)
- Increase timeout (40 attempts)
- Check Serial Monitor for errors
```

### 5. Admin Panel Access নাই
```
Problem: Can't access /admin during Evil Twin
Solution:
- Use exact URL: http://192.168.4.1/admin
- Bookmark it before starting Evil Twin
- Check if connected to ESP32 AP
- Try 192.168.4.1 IP directly
```

---

## 📈 Performance

### Specs
```
✅ RAM Usage: 15.1% (49.5KB)
✅ Flash Usage: 62.2% (814KB)
✅ Scan Speed: 300ms
✅ Page Load: <200ms
✅ DNS Response: <10ms
✅ Max Clients: 8
✅ WiFi Range: 50-100m
```

---

## ⚠️ Legal Notice

### শিক্ষামূলক উদ্দেশ্যে Only!

```
✅ নিজের network এ test করুন
✅ Permission নিয়ে testing করুন
✅ Security research এর জন্য

❌ অনুমতি ছাড়া ব্যবহার করবেন না
❌ অবৈধ উদ্দেশ্যে নয়
❌ অন্যের privacy লঙ্ঘন করবেন না
```

---

## 📚 More Info

বিস্তারিত জানতে দেখুন:
- **[README.md](README.md)** - Complete documentation
- **[FEATURES.md](FEATURES.md)** - All features explained

---

**⚡ Quick Start Guide - ESP32 Evil Twin v4.3**

*Get started in 5 minutes!* 🚀
- Max Clients: 8
- RAM Usage: ~50KB

### Performance
- Scan Speed: 300ms
- Page Load: <200ms
- Credential Capture: Instant

---

## 🎯 Common Tasks

### Change Admin Credentials
```cpp
// File: src/main.cpp (Line ~13)
const char* adminSSID = "YourSSID";
const char* adminPassword = "YourPassword";
```

### Increase Network Limit
```cpp
// File: src/main.cpp (Line ~19)
String networkSSIDs[30];  // Change to 50
```

### Adjust WiFi Power
```cpp
// File: src/main.cpp (Line ~467)
WiFi.setTxPower(WIFI_POWER_19_5dBm);  // Max
WiFi.setTxPower(WIFI_POWER_15dBm);    // Medium
```

---

## 🐛 Quick Troubleshooting

### Upload Failed
```
✓ Hold BOOT button during upload
✓ Check USB cable
✓ Select correct COM port
```

### Can't Connect to Admin AP
```
✓ Check SSID: Unknown
✓ Password: Rabbi0606@
✓ Restart ESP32
```

### Captive Portal Not Working
```
✓ Use Open network (no password)
✓ Check DNS server in Serial Monitor
✓ Clear device cache
```

### Evil Twin Not Visible
```
✓ Try different channel
✓ Increase WiFi power
✓ Move closer to ESP32
```

---

## 💡 Pro Tips

### 1. Best SSID Names
```
✅ Free_WiFi
✅ Starbucks_WiFi
✅ Airport_WiFi
✅ Hotel_Guest
❌ MyEvilTwin (obvious!)
```

### 2. Optimal Channel
```
1-6: Commonly used (crowded)
7-11: Less crowded
Use same channel as target for best results
```

### 3. Signal Strength
```
Place ESP32 closer to victims than real AP
Higher signal = More victims connect
```

### 4. Admin Access
```
Bookmark: http://192.168.4.1/admin
Saves time during Evil Twin operation
```

---

## 📱 Victim Experience

### Step by Step
```
1. 📶 Sees "Free_WiFi" network
2. 🔓 Connects (no password!)
3. 📲 Gets notification: "Sign in to network"
4. 🌐 Opens login page automatically
5. 🔑 Enters WiFi password
6. ✅ Shows "Connected" message
7. 💾 Password saved to ESP32!
```

### What Victims See
```html
🔐 WiFi Authentication Required
Please enter password to connect to

[Free_WiFi]

[Password: ________]
[Connect to WiFi]

⚠️ TRAINING DEMO
This is educational demonstration.
Never enter credentials on unfamiliar networks.
```

---

## 🔒 Safety Guidelines

### ✅ DO:
- Test on your own devices
- Get permission before testing
- Use for educational purposes
- Follow local laws
- Secure captured data

### ❌ DON'T:
- Target public networks without permission
- Store passwords insecurely
- Use for malicious purposes
- Violate privacy laws
- Share captured credentials

---

## 📊 Serial Monitor Output

### Normal Boot
```
╔══════════════════════════════════════════╗
║   ESP32 EVIL TWIN TRAINER (OPTIMIZED)   ║
║   Version 4.0 - Evil Twin Only           ║
╚══════════════════════════════════════════╝

✅ ADMIN ACCESS POINT CREATED
   SSID: Unknown
   Password: Rabbi0606@
   IP: 192.168.4.1

🌐 WEB SERVER STARTED
   Dashboard: http://192.168.4.1

[SCAN] Found 12 networks

🚀 SYSTEM READY
```

### Evil Twin Active
```
[EVIL TWIN] Creating Evil Twin AP...
[EVIL TWIN] ✅ Created successfully!
  SSID: Starbucks_WiFi
  Password: (Open - No Password)
  Channel: 6
  IP: 192.168.4.1
  📱 Victims: http://192.168.4.1
  🔐 Admin: http://192.168.4.1/admin
```

### Credential Captured
```
[CREDENTIAL] 📧 Captured!
  Time: 00:05:23
  SSID: Starbucks_WiFi
  Password: MyPassword123
  Client IP: 192.168.4.2
```

---

## 🎓 Educational Use Cases

### 1. Security Awareness Training
```
Show employees how Evil Twin attacks work
Demonstrate captive portal risks
Teach WiFi security best practices
```

### 2. Penetration Testing
```
Test organization's WiFi security
Verify captive portal detection
Check device security awareness
```

### 3. Research & Development
```
Study captive portal mechanisms
Test multi-platform compatibility
Develop security countermeasures
```

### 4. IoT Security Testing
```
Test device WiFi security
Verify secure connection handling
Check credential protection
```

---

## 🔄 Upgrade from v3.0

### What Changed
```
❌ Removed: Deauth Attack
❌ Removed: Packet Injection
❌ Removed: Complex UI
✅ Added: DNS Server
✅ Added: Better Captive Portal
✅ Optimized: Memory Usage
✅ Simplified: Code Structure
```

### Migration Steps
```
1. Backup old credentials (if any)
2. Note admin password (changed)
3. Update bookmark to /admin
4. Upload new code
5. Test Evil Twin creation
```

---

## 📞 Quick Help

### Error: Upload Failed
```bash
# Check PlatformIO installation
pio --version

# Update platform
pio platform update espressif32
```

### Error: Low Memory
```
[MEM] Free: 25000 bytes

Solutions:
1. Reduce credential array to 30
2. Reduce network array to 20
3. Clear old captured data
```

### Error: DNS Not Working
```
Check Serial Monitor for:
[DNS] Started on port 53

If not showing:
1. Restart ESP32
2. Check Evil Twin is active
3. Verify client connection
```

---

## 🎯 Performance Tips

### Maximize Speed
```cpp
// Use QIO flash mode (platformio.ini)
board_build.flash_mode = qio

// Optimize WiFi buffers
-DCONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM=16
```

### Save Memory
```cpp
// Reduce arrays
String networkSSIDs[20];  // Instead of 30
Credential capturedCreds[30];  // Instead of 50
```

### Improve Range
```cpp
// Maximum WiFi power
WiFi.setTxPower(WIFI_POWER_19_5dBm);

// Use better antenna (if available)
```

---

## 📚 Learn More

### Files to Study
```
src/main.cpp              - Main code (550 lines)
platformio.ini            - Build settings
README.md                 - Full documentation
OPTIMIZATION_SUMMARY.md   - Detailed changes
```

### Key Functions
```cpp
scanNetworks()           - WiFi scanning
createEvilTwinAP()       - Evil Twin creation
setupServer()            - Web server routes
dnsServer.processNextRequest() - Captive portal
```

### Important Variables
```cpp
isEvilTwinActive         - Evil Twin status
evilTwinSSID             - Current Evil Twin name
credCount                - Number of captured credentials
networkCount             - Number of scanned networks
```

---

## ✅ Quick Checklist

### Before Starting
- [ ] ESP32 connected via USB
- [ ] PlatformIO installed in VS Code
- [ ] Code uploaded successfully
- [ ] Serial Monitor working

### Testing Evil Twin
- [ ] Can connect to admin AP
- [ ] Can access http://192.168.4.1
- [ ] Network scan works
- [ ] Can create Evil Twin
- [ ] `/admin` URL bookmarked

### Verifying Captive Portal
- [ ] Victim device connects to Evil Twin
- [ ] Gets "Sign in" notification
- [ ] Login page opens automatically
- [ ] Password capture works
- [ ] Shows in admin dashboard

---

## 🎉 You're Ready!

এখন আপনি **ESP32 Evil Twin Trainer** ব্যবহার করার জন্য সম্পূর্ণ প্রস্তুত!

### Next Steps:
1. ✅ Upload the code
2. ✅ Connect to admin AP
3. ✅ Scan networks
4. ✅ Create Evil Twin
5. ✅ Test captive portal
6. ✅ View captured credentials

---

**Remember: Use for education only! 📚**

*Knowledge is power - use it responsibly!* 💪
