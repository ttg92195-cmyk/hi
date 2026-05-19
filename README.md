# 🏎️ Racing Game - Android APK

A racing game for Android built with C++ game engine + Java UI.

## ⬇️ Download APK

Go to the **[Releases Page](https://github.com/ttg92195-cmyk/hi/releases)** and download the APK file!

### Install Steps
1. Download the `.apk` file from Releases
2. Enable **"Install from unknown sources"** in phone settings
3. Open the downloaded APK to install
4. Play and enjoy! 🎮

## 🕹️ Controls

| Action | How |
|---|---|
| **Steer** | Touch & drag left/right |
| **Nitro** | Tap N2O button (bottom right) |
| **Pause** | Tap || button (top right) |
| **Restart** | Tap RESTART button (after Game Over) |

## 🎮 Gameplay

- 🚗 Avoid **red cars** (slow), **purple cars** (fast), **orange trucks**
- 💰 Collect **gold coins** for +50 points each
- 🛡️ Get a **shield** after being hit (temporary invincibility)
- ❤️ You have **3 lives**
- ⚡ Use **Nitro** for speed boost
- 📈 Speed increases over time - how long can you survive?

## 🏗️ Architecture

```
┌─────────────────────────────┐
│     Java (Android UI)       │
│  MainActivity + GameView    │
│  - Touch Input              │
│  - Canvas Rendering         │
│  - Game Thread (60 FPS)     │
├───────────┬─────────────────┤
│   JNI     │  Bridge Layer   │
├───────────▼─────────────────┤
│     C++ Game Engine         │
│  - Game Logic               │
│  - Collision Detection      │
│  - Obstacle Spawning        │
│  - Score & State            │
└─────────────────────────────┘
```

## 📁 Project Structure

```
app/src/main/
├── AndroidManifest.xml
├── java/com/racing/game/
│   ├── MainActivity.java      # Activity (fullscreen)
│   └── GameView.java          # Game rendering + input
├── cpp/
│   ├── CMakeLists.txt         # Native build config
│   ├── game_engine.h          # C++ game engine
│   └── jni_bridge.cpp         # JNI bridge
└── res/
    └── values/
        ├── strings.xml
        └── styles.xml
```

## 🔨 Build from Source

```bash
# Clone the repo
git clone https://github.com/ttg92195-cmyk/hi.git
cd hi

# Build debug APK
./gradlew assembleDebug

# APK will be in app/build/outputs/apk/debug/
```

## Requirements
- Android 5.0+ (API 21)
- C++11 compatible NDK

## License
MIT License
