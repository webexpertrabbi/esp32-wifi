# 📚 ESP32 Evil Twin - সম্পূর্ণ Features Documentation

## 📋 সূচিপত্র

1. [🔄 Auto-Connect & Password Validation](#auto-connect)
2. [📱 Android WiFi Dialog Design](#android-dialog)
3. [⚙️ Admin Settings](#admin-settings)
4. [👥 Evil Twin Attack](#evil-twin)

---

<a name="auto-connect"></a>
## 🔄 Auto-Connect & Password Validation (v4.3)

### কিভাবে কাজ করে

User password দিলে ESP32 স্বয়ংক্রিয়ভাবে original WiFi network এ connect attempt করে।

**সঠিক Password (✅):**
```
1. ESP32 original network এ connect হয়
2. Evil Twin automatic stop হয়
3. User original network এ reconnect করে
4. User internet পায়!
5. Dialog 3 seconds পর close হয়
```

**ভুল Password (❌):**
```
1. Connection fail হয়
2. Evil Twin চলতে থাকে
3. User generic success message দেখে
4. User আবার try করতে পারে
```

### Serial Monitor Output

**Success:**
```
[CREDENTIAL] 📧 Captured!
  SSID: MyWiFi
  Password: CorrectPass123

[AUTO-CONNECT] Attempting to connect...
[AUTO-CONNECT] ✅ Password is CORRECT!
[AUTO-STOP] ✅ Evil Twin stopped
[AUTO-STOP] ✅ Victim will connect to original network

╔══════════════════════════════════════════╗
║   PASSWORD VALIDATED & ATTACK STOPPED   ║
╚══════════════════════════════════════════╝
```

**Failure:**
```
[AUTO-CONNECT] ❌ Password is INCORRECT
[AUTO-CONNECT] Evil Twin continues running...
```

### Benefits

- ✅ Automatic password validation
- ✅ User gets real internet (if password correct)
- ✅ No manual testing needed
- ✅ Seamless user experience
- ✅ Higher success rate

---

<a name="android-dialog"></a>
## 📱 Android WiFi Dialog (v4.2)

### 100% রিয়েলিস্টিক Design

Evil Twin login page এখন exact Android WiFi dialog replica!

**Features:**
- ✅ Black background (Android dark theme)
- ✅ Network name at top
- ✅ Password field with eye icon (working toggle)
- ✅ Auto reconnect toggle (animated)
- ✅ View more dropdown
- ✅ Material Design button
- ✅ Mobile optimized

### UI Elements

**1. Header Bar**
```
[‹] Network Name
```
- Dark gray background (#1a1a1a)
- Back button (cosmetic, keeps user on page)

**2. Password Field**
```
Password
[Enter password_________ 👁]
```
- Bottom border only (Material Design)
- Eye icon toggles show/hide
- Blue focus state (#4a90e2)

**3. Auto Reconnect Toggle**
```
Auto reconnect    [🔵 ON]
```
- Animated switch
- Blue when ON, gray when OFF

**4. Connect Button**
```
[    Connect    ]
```
- Rounded corners (25px)
- Dark blue (#2d5a8c)
- Full width

### Color Palette

```css
Background:    #000000 (Black)
Container:     #1a1a1a (Dark Gray)
Text:          #ffffff (White)
Focus:         #4a90e2 (Blue)
Button:        #2d5a8c (Dark Blue)
```

### Why It Works

1. **Familiarity** - Everyone knows Android WiFi dialog
2. **Trust** - Native UI = trusted
3. **Muscle Memory** - Auto-fill password
4. **No Suspicion** - Looks completely real

---

<a name="admin-settings"></a>
## ⚙️ Admin Settings (v4.1)

### Configurable Options

Dashboard থেকে সরাসরি configuration পরিবর্তন করতে পারবেন:

**1. Admin SSID**
- আপনার Admin AP এর WiFi নাম
- ডিফল্ট: "Unknown"
- উদাহরণ: "MyESP32", "AdminPanel"

**2. Admin Password**
- Admin AP এ connect করার password
- ডিফল্ট: "Rabbi0606@"
- মিনিমাম 8 characters

**3. Hide Admin SSID**
- **Show** - সবাই SSID দেখতে পাবে (default)
- **Hide** - SSID লুকানো থাকবে

### কিভাবে ব্যবহার করবেন

**Step 1: Settings খুলুন**
```
http://192.168.4.1/admin → Scroll down → SETTINGS
```

**Step 2: পরিবর্তন করুন**
```
Admin SSID: [MyPrivateESP]
Admin Password: [MySecure@Pass]
Hide SSID: [Show/Hide]
```

**Step 3: সেভ করুন**
```
"SAVE SETTINGS" → Restart ESP32
```

**Step 4: নতুন Settings দিয়ে Connect**
```
WiFi: MyPrivateESP
Password: MySecure@Pass
Dashboard: http://192.168.4.1/admin
```

### Reset to Default

**Dashboard থেকে:**
```
"RESET TO DEFAULT" → Restart
Default: Unknown / Rabbi0606@
```

**Flash Erase (যদি password ভুলে যান):**
```bash
pio run -t erase
pio run -t upload
```

### Technical Details

**Storage:** ESP32 Flash Memory (Preferences library)

**Boot Process:**
```
1. Flash থেকে settings load
2. না পাওয়া গেলে default use
3. Admin AP তৈরি করে saved settings দিয়ে
```

---

<a name="evil-twin"></a>
## 👥 Evil Twin Attack

### কিভাবে কাজ করে

**Attack Flow:**
```
1️⃣ Real Network Scan
   └─> "Starbucks_WiFi" found (Channel 6)

2️⃣ Evil Twin Create
   └─> "Starbucks_WiFi" clone (Ch 6, Open)
   └─> Stronger signal

3️⃣ DNS Hijacking
   └─> All requests → 192.168.4.1
   └─> google.com → Login Page

4️⃣ Captive Portal
   └─> Android/iOS auto-detect
   └─> "Sign in to network" notification

5️⃣ Credential Capture
   └─> User enters password
   └─> Instantly saved

6️⃣ Auto-Validation (v4.3)
   └─> Tests password on real network
   └─> Success → Auto-stop & reconnect
   └─> Failure → Continue attack
```

### Features

**Evil Twin AP:**
- Open Network (no password)
- Same SSID as target
- Same channel as target
- Stronger signal preferred

**Captive Portal:**
- Auto-redirect to login page
- Android WiFi dialog design
- DNS hijacking (all domains → login)

**Credential Capture:**
- Real-time save
- Timestamp logging
- Client IP tracking
- Admin panel display

**Auto-Validation:**
- Tests password automatically
- Connects to real network if correct
- Auto-stops attack on success
- User gets internet seamlessly

### Why Open Network?

```
✅ Open Network:
   - Users trust "Free WiFi"
   - No password barrier
   - Higher victim count
   - Faster connection

❌ Password Protected:
   - Users suspicious
   - Need to know password
   - Lower success rate
```

### Best Practices

1. **SSID Selection:**
   - Common names (Free_WiFi, Starbucks)
   - Same as target network
   - No special characters

2. **Channel Selection:**
   - Same as target network
   - Less congested channels
   - Monitor interference

3. **Signal Strength:**
   - Stronger than target (if possible)
   - Close to target area
   - Maximum power (19.5dBm)

4. **Timing:**
   - Peak hours = more victims
   - Target busy locations
   - Monitor Serial output

### Supported Platforms

| Platform | Captive Portal | Auto-Detect |
|----------|----------------|-------------|
| Android | ✅ | ✅ |
| iOS | ✅ | ✅ |
| Windows | ✅ | ✅ |
| macOS | ✅ | ✅ |
| Linux | ✅ | ⚠️ (varies) |

---

## 🔧 Configuration Options

### WiFi Power

```cpp
// Maximum power (default)
WiFi.setTxPower(WIFI_POWER_19_5dBm);

// Medium power (battery save)
WiFi.setTxPower(WIFI_POWER_15dBm);

// Low power
WiFi.setTxPower(WIFI_POWER_11dBm);
```

### Network Storage

```cpp
// Default: 30 networks
String networkSSIDs[30];

// Increase if needed:
String networkSSIDs[50];  // More networks
```

### Credential Storage

```cpp
// Default: 50 credentials
Credential capturedCreds[50];

// Increase if needed:
Credential capturedCreds[100];  // More storage
```

### Connection Timeout

```cpp
// Default: 10 seconds
int attempts = 0;
while(attempts < 20 && WiFi.status() != WL_CONNECTED) {
    delay(500);  // 500ms × 20 = 10s
    attempts++;
}

// Adjust:
// Fast: attempts < 10  (5 seconds)
// Slow: attempts < 40  (20 seconds)
```

---

## 📊 Performance Metrics

### Memory Usage

| Component | Size | Status |
|-----------|------|--------|
| RAM | 49.5KB | 15.1% used |
| Flash | 814KB | 62.2% used |
| Networks | 30 items | ~6KB |
| Credentials | 50 items | ~10KB |

### Speed

```
✅ Network Scan: 300ms
✅ Page Load: <200ms
✅ DNS Response: <10ms
✅ Credential Save: <5ms
✅ Password Test: <10s
```

### Capacity

```
✅ Max Clients: 8 simultaneous
✅ Max Networks: 30 stored
✅ Max Credentials: 50 stored
✅ WiFi Range: 50-100m (open space)
```

---

## 🐛 Troubleshooting

### Evil Twin দেখা যাচ্ছে না

**সমাধান:**
```
1. WiFi power check করুন (19.5dBm?)
2. Channel congestion check করুন
3. Different channel try করুন
4. ESP32 closer to target area
```

### Captive Portal কাজ করছে না

**সমাধান:**
```
1. DNS server running check করুন
2. Open network (no password) verify করুন
3. Device cache clear করুন
4. Serial Monitor check করুন
```

### Password Validation ব্যর্থ হচ্ছে

**সমাধান:**
```
1. Original WiFi range এ আছে কিনা
2. WPA2 network কিনা (WPA3 supported না)
3. Timeout বাড়ান (20-40 attempts)
4. Network congestion check করুন
```

### Admin AP Restore হচ্ছে না

**সমাধান:**
```
1. 5 seconds wait করুন (auto-restore delay)
2. ESP32 manually restart করুন
3. Serial Monitor check করুন errors এর জন্য
```

---

## ⚠️ Legal & Ethical Guidelines

### ⚠️ শিক্ষামূলক উদ্দেশ্যে Only!

**আইনি সতর্কতা:**
- ✅ নিজের network এ test করুন
- ❌ অনুমতি ছাড়া অন্যের network এ use করবেন না
- ✅ WiFi security awareness এর জন্য
- ❌ অবৈধ উদ্দেশ্যে নয়

**নৈতিক নির্দেশিকা:**
- Permission নিয়ে testing
- Captured data সুরক্ষিত রাখুন
- Responsible disclosure
- Security research only

**Use Cases:**
1. Educational demonstrations
2. Security auditing (with permission)
3. WiFi security research
4. Penetration testing (authorized)
5. Network security training

---

## 🎯 Version History

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
- ✅ 60% memory savings
- ✅ Minimal terminal UI

---

**📚 Complete Features Documentation**

*Educational WiFi Security Research Tool* 🔐
