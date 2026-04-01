# Project Audit: Captive-WiFi-Portal

## Overview
**Project Name**: Captive-WiFi-Portal  
**Directory**: `/Users/kartik/Documents/Captive-WiFi-Portal`  
**Purpose**: An ESP32-based Evil Twin / Rogue Access Point project that mimics the "KIET" Wi-Fi login network. It sets up a captive portal to intercept and steal user credentials via a locally hosted phishing page.

> [!WARNING]
> This project implements a credential harvesting (phishing) attack using a Rogue Access Point. It is assumed to be for educational, authorized penetration testing, or CTF purposes. Unauthorized use of this to capture user credentials is a malicious cyber attack.

## File Structure
The repository is simple, consisting of the following core files:
- `README.md`: Contains setup instructions, prerequisites, and a basic explanation of how the attack works.
- `kiet_phish.ino`: The main Arduino/C++ source code for the ESP32 device.
- `data/index.html`: The HTML and inline CSS for the fake captive portal login page.
- `data/kiet_logo.png`: An image asset used in the phishing page to increase its credibility.

---

## Detailed Component Audit

### 1. `kiet_phish.ino` (Backend Logic & Network Setup)

This is the main driver running on an ESP32 Wroom module.

**Key Functionalities & Analysis:**
- **Network Configuration**:
  - Sets up an open wireless access point (AP) named `"KIET"`.
  - Configures a static IP address: `172.16.16.16`.
- **Captive Portal / DNS Hijacking**:
  - Uses the `DNSServer` library to respond to all DNS queries on port 53, routing any domain requested by the connected victim back to the ESP32's IP (`172.16.16.16`).
  - Implements OS-specific captive portal detection endpoints:
    - Android: `/generate_204`
    - Windows: `/fwlink`
    - macOS/iOS: `/hotspot-detect.html`
  - A catch-all `server.onNotFound` handler ensures any unhandled HTTP traffic redirects to `/index.html`.
- **File Serving & Storage**:
  - Uses `SPIFFS` (Serial Peripheral Interface Flash File System) to store and serve the phishing page (`index.html`) and log credentials.
- **Credential Harvesting**:
  - A POST route is exposed at `/login` to handle form submissions from the phishing page.
  - The route parses `username` and `password` parameters, prints them to the Serial Monitor (for real-time observation if connected via USB).
  - It saves the captured credentials to a persistent file in SPIFFS memory: `/creds.txt`.
  - It then sends back a basic HTTP 200 response: `<h2>Error! Please try again after some time.</h2>`, to avoid raising the victim's suspicion further.

**Vulnerabilities & Technical Feedback for the Code**:
- **Lack of Input Sanitization**: The username and password values pulled from the POST parameters are directly concatenated into the SPIFFS `creds.txt` file and `Serial.println`. This poses a risk if someone inputs a very large payload or unexpected characters, though exploitability is low on a simple ESP32 environment.
- **No Data Retrieval Interface**: The code saves credentials into `/creds.txt`, but to read this data without a serial connection, the attacker would have to write custom code or mount the SPIFFS; there is no exposed route like `/view_creds` to retrieve harvested data over the air.
- **Plaintext Storage**: Captured credentials are mathematically logged in plaintext into `/creds.txt`.

### 2. `data/index.html` (Phishing Page)

This file contains the front-end interface that victims interact with.

**Key Functionalities & Analysis:**
- **Styling**: It uses internal CSS (`<style>`) and variable color tokens to replicate the look and feel of the genuine KIET internet login portal.
- **Form Action**: Contains a standard `<form>` element configured to execute a `POST` request to `/login`. The input fields are named `username` and `password`, which maps precisely to what `kiet_phish.ino` looks for.
- **Social Engineering Elements**:
  - Includes various seemingly-functional links ("KIET ERP Portal", "KIET Email", etc.) leading to actual KIET subdomains, increasing legitimacy.
  - Features the `kiet_logo.png` image.
  - Shows fake support hotline numbers and emails, discouraging users from contacting their IT support effectively out of band.

**Vulnerabilities & Technical Feedback for the Front-End**:
- The HTML structure is fairly solid for its purpose, rendering adequately on both desktop and mobile devices via some basic media query adjustments (e.g., `@media (max-width: 480px)`).

---

## Conclusion & Recommendations
The project is a fully functional, minimal, yet highly effective Evil Twin phishing attack tool tailored for a specific target ("KIET"). 

If you are developing this to demonstrate network insecurity (e.g. at an institute or for a CTF):
1. **Expand Readme**: Consider documenting how an authenticated user retrieves `/creds.txt` cleanly after deploying the ESP32.
2. **Handle Errors better**: The abrupt `<h2>Error!...</h2>` response upon a successful phishing attempt might raise suspicion. The script could instead serve a more realistic and stylized error page, or redirect the user dynamically to the actual KIET domain using a JavaScript timeout.
