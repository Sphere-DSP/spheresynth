#pragma once
#include <JuceHeader.h>

namespace SphereSynthResources {

    inline const juce::String css = R"CSS(
      * { margin: 0; padding: 0; box-sizing: border-box;
        -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none;
        -webkit-touch-callout: none; -webkit-tap-highlight-color: transparent;
      }
      *::selection { background: transparent; }
      *::-moz-selection { background: transparent; }
      body { 
        background: linear-gradient(135deg, #000000 0%, #000510 100%);
        color: white; 
        font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
        display: flex; flex-direction: column; align-items: center; justify-content: space-between; 
        height: 100vh; overflow: hidden; position: relative; cursor: default;
      }
      .top-nav { position: fixed; top: 0; left: 0; width: 100%; height: 60px;
        display: flex; justify-content: space-between; align-items: center; padding: 0 20px;
        background: rgba(0, 0, 0, 0.6); backdrop-filter: blur(20px); z-index: 2000;
        border-bottom: 1px solid rgba(0, 229, 255, 0.1);
      }
      .nav-center { display: flex; gap: 8px; position: absolute; left: 50%; transform: translateX(-50%); }
      .view-btn { background: rgba(255, 255, 255, 0.05); border: 1px solid rgba(120, 120, 130, 0.2);
        color: rgba(255, 255, 255, 0.7); padding: 8px 24px; border-radius: 4px; font-size: 14px;
        font-weight: 600; cursor: pointer; text-transform: uppercase; letter-spacing: 1.5px;
      }
      .view-btn:hover { background: rgba(255, 255, 255, 0.1); }
      .view-btn.active { background: rgba(0, 229, 255, 0.15); border-color: rgba(0, 229, 255, 0.5); color: #00e5ff; }
      .view-container { display: none; width: 100%; height: 100%; }
      .view-container.active { display: block; }
      .header { margin-top: 100px; text-align: center; z-index: 10; position: relative; }
      .tutorial-btn, .settings-btn { background: rgba(255, 255, 255, 0.1); border: 1px solid rgba(120, 120, 130, 0.3);
        color: rgba(255, 255, 255, 0.9); padding: 8px 16px; border-radius: 4px; font-size: 11px;
        font-weight: 600; cursor: pointer; text-transform: uppercase; letter-spacing: 1px;
      }
      h1 { font-size: clamp(24px, 5vw, 52px); font-weight: 200; letter-spacing: clamp(6px, 1.5vw, 14px);
        color: #ffffff; text-shadow: 0 0 20px rgba(255, 255, 255, 0.5); margin-bottom: 8px;
      }
      .sphere-container { position: fixed; top: 80px; left: 0; width: 100%; bottom: 220px;
        z-index: 1; display: flex; align-items: center; justify-content: center;
      }
      canvas { display: block; width: 100%; height: 100%; }
      .controls { display: flex; gap: 16px; z-index: 10; position: fixed; bottom: 130px;
        left: 50%; transform: translateX(-50%);
      }
      button { background: rgba(255, 255, 255, 0.05); color: rgba(255, 255, 255, 0.9);
        border: 1px solid rgba(0, 229, 255, 0.3); padding: 10px 20px; border-radius: 20px;
        font-size: 11px; font-weight: 600; letter-spacing: 1px; cursor: pointer; text-transform: uppercase;
      }
      button:hover { border-color: rgba(0, 229, 255, 0.8); box-shadow: 0 0 20px rgba(0, 229, 255, 0.3); }
      button.active { background: linear-gradient(135deg, rgba(0, 229, 255, 0.3), rgba(0, 200, 255, 0.2));
        border-color: #00e5ff; color: white;
      }
      .keyboard-container { width: 100%; display: flex; justify-content: center; padding: 0 20px;
        z-index: 10; position: fixed; bottom: 20px; left: 0;
      }
      .keyboard { display: flex; height: 90px; background: linear-gradient(180deg, #1a1a1a, #0a0a0a);
        padding: 10px 16px; border-radius: 16px; box-shadow: 0 10px 40px rgba(0, 0, 0, 0.7); gap: 3px;
      }
      .key { flex: 1; background: linear-gradient(180deg, #fff, #e8e8e8); border: 1px solid #333;
        border-radius: 0 0 8px 8px; cursor: pointer; min-width: 24px;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3); position: relative;
      }
      .key.black { background: linear-gradient(180deg, #2a2a2a, #0a0a0a); height: 55%;
        width: 24px; flex: 0 0 24px; margin: 0 -12px; z-index: 10; border-radius: 0 0 6px 6px;
      }
      .key:active, .key.pressed { background: linear-gradient(180deg, #00e5ff, #00b8d4);
        box-shadow: 0 0 30px rgba(0, 229, 255, 0.8);
      }
      .volume-meter-container { position: fixed; right: 20px; top: 50%; transform: translateY(-50%);
        width: 4px; height: 300px; background: rgba(255, 255, 255, 0.1); border-radius: 2px; z-index: 100;
      }
      .volume-meter-bar { width: 100%; height: 0%; background: linear-gradient(to top, #00e5ff, #00ff88);
        position: absolute; bottom: 0; box-shadow: 0 0 15px #00e5ff;
      }
      .footer-credit { position: fixed; bottom: 10px; left: 10px; font-size: 10px;
        color: rgba(255, 255, 255, 0.4); letter-spacing: 2px; text-transform: uppercase; z-index: 1000;
        pointer-events: none;
      }
      .tutorial-popup { position: absolute; background: rgba(10, 10, 10, 0.95); backdrop-filter: blur(20px);
        border: 2px solid rgba(0, 229, 255, 0.5); border-radius: 16px; padding: 20px; max-width: 300px;
        z-index: 3000; display: none;
      }
      .tutorial-popup.active { display: block; }
      .popup-title { color: #00e5ff; font-size: 16px; font-weight: 700; margin-bottom: 10px; }
      .popup-text { color: rgba(255, 255, 255, 0.8); font-size: 13px; line-height: 1.6; margin-bottom: 15px; }
      .popup-dismiss { background: rgba(0, 229, 255, 0.2); border: 1px solid #00e5ff; color: white;
        padding: 8px 16px; border-radius: 20px; font-size: 12px; cursor: pointer; width: 100%;
        text-transform: uppercase; letter-spacing: 1px;
      }
      .settings-select { width: 100%; background: rgba(30, 30, 30, 0.8); border: 1px solid rgba(0, 229, 255, 0.3);
        color: white; padding: 10px; border-radius: 8px; font-size: 12px; cursor: pointer;
      }
      #popup-sphere { top: 200px; left: 50%; transform: translateX(-50%); }
      #popup-controls { top: 420px; left: 50%; transform: translateX(-50%); }
      #popup-keyboard { bottom: 200px; left: 50%; transform: translateX(-50%); }
      .fx-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(160px, 1fr)); gap: 20px;
        padding: 100px 40px 40px; max-width: 1200px; margin: 0 auto; width: 100%;
      }
      .fx-card { background: rgba(10, 15, 25, 0.8); backdrop-filter: blur(20px);
        border: 1px solid rgba(0, 229, 255, 0.15); border-radius: 12px; padding: 16px; cursor: pointer;
        aspect-ratio: 1; display: flex; flex-direction: column; justify-content: space-between;
      }
      .fx-card:hover { border-color: rgba(0, 229, 255, 0.5); transform: translateY(-4px);
        box-shadow: 0 10px 40px rgba(0, 229, 255, 0.15);
      }
      .fx-card-header { display: flex; justify-content: space-between; align-items: center; }
      .fx-card-title { font-size: 14px; font-weight: 600; letter-spacing: 1.5px; color: rgba(255, 255, 255, 0.9);
        text-transform: uppercase;
      }
      .fx-toggle { width: 36px; height: 20px; background: rgba(255, 255, 255, 0.1); border-radius: 10px;
        position: relative; cursor: pointer; border: 1px solid rgba(255, 255, 255, 0.2);
      }
      .fx-toggle::after { content: ''; position: absolute; width: 14px; height: 14px;
        background: rgba(255, 255, 255, 0.6); border-radius: 50%; top: 2px; left: 2px;
      }
      .fx-toggle.on { background: rgba(0, 229, 255, 0.3); border-color: rgba(0, 229, 255, 0.6); }
      .fx-toggle.on::after { left: 18px; background: #00e5ff; box-shadow: 0 0 10px #00e5ff; }
      .fx-card-preview { flex: 1; display: flex; align-items: center; justify-content: center; margin: 12px 0; }
      .eq-mini-display { width: 100%; height: 60px; }
      .eq-mini-canvas { width: 100%; height: 100%; border-radius: 6px; background: rgba(0, 0, 0, 0.3); }
      .eq-panel { position: fixed; top: 60px; left: 0; right: 0; bottom: 0;
        background: rgba(5, 10, 20, 0.98); backdrop-filter: blur(30px); z-index: 1500;
        display: none; flex-direction: column; overflow: hidden;
      }
      .eq-panel.active { display: flex; }
      .eq-header { display: flex; justify-content: space-between; align-items: center;
        padding: 12px 20px; border-bottom: 1px solid rgba(0, 229, 255, 0.1); flex-shrink: 0;
      }
      .eq-title { font-size: 16px; font-weight: 600; letter-spacing: 3px; color: rgba(255, 255, 255, 0.9);
        text-transform: uppercase;
      }
      .eq-close-btn { background: rgba(255, 255, 255, 0.05); border: 1px solid rgba(255, 255, 255, 0.2);
        color: rgba(255, 255, 255, 0.8); width: 32px; height: 32px; border-radius: 50%;
        font-size: 20px; cursor: pointer; display: flex; align-items: center; justify-content: center; padding: 0;
        line-height: 1;
      }
      .eq-close-btn:hover { background: rgba(255, 255, 255, 0.1); border-color: rgba(0, 229, 255, 0.5);
        color: #00e5ff; transform: none;
      }
      .eq-display-container { flex: 1; padding: 10px 15px; display: flex; flex-direction: column; gap: 10px; overflow: hidden; min-height: 0; }
      .eq-analyzer { flex: 1; background: rgba(0, 0, 0, 0.4); border: 1px solid rgba(0, 229, 255, 0.1);
        border-radius: 10px; position: relative; overflow: hidden; min-height: 100px;
      }
      .eq-analyzer-canvas { width: 100%; height: 100%; }
      .eq-grid-overlay { position: absolute; top: 0; left: 0; right: 0; bottom: 0; pointer-events: none;
        background-image: linear-gradient(rgba(0, 229, 255, 0.03) 1px, transparent 1px),
          linear-gradient(90deg, rgba(0, 229, 255, 0.03) 1px, transparent 1px);
        background-size: 50px 30px;
      }
      .eq-freq-labels { position: absolute; bottom: 5px; left: 10px; right: 10px;
        display: flex; justify-content: space-between; font-size: 9px; color: rgba(255, 255, 255, 0.3);
        pointer-events: none;
      }
      .eq-db-labels { position: absolute; top: 10px; bottom: 25px; left: 8px;
        display: flex; flex-direction: column; justify-content: space-between; font-size: 9px;
        color: rgba(255, 255, 255, 0.3); pointer-events: none;
      }
      .eq-bands-container { display: flex; gap: 8px; padding: 8px 0; overflow-x: auto; flex-shrink: 0; }
      .eq-band { flex: 0 0 110px; background: rgba(15, 20, 30, 0.8);
        border: 1px solid rgba(0, 229, 255, 0.15); border-radius: 8px; padding: 8px;
        display: flex; flex-direction: column; gap: 5px;
      }
      .eq-band:hover { border-color: rgba(0, 229, 255, 0.4); }
      .eq-band-header { display: flex; justify-content: space-between; align-items: center; }
      .eq-band-num { font-size: 10px; font-weight: 700; color: #00e5ff; letter-spacing: 1px; }
      .eq-band-toggle { width: 26px; height: 14px; background: rgba(255, 255, 255, 0.1);
        border-radius: 7px; position: relative; cursor: pointer;
      }
      .eq-band-toggle::after { content: ''; position: absolute; width: 10px; height: 10px;
        background: rgba(255, 255, 255, 0.5); border-radius: 50%; top: 2px; left: 2px;
      }
      .eq-band-toggle.on { background: rgba(0, 229, 255, 0.3); }
      .eq-band-toggle.on::after { left: 14px; background: #00e5ff; box-shadow: 0 0 6px #00e5ff; }
      .eq-param { display: flex; flex-direction: column; gap: 2px; }
      .eq-param-label { font-size: 8px; color: rgba(255, 255, 255, 0.4); text-transform: uppercase; letter-spacing: 0.5px; }
      .eq-param-value { font-size: 11px; color: rgba(255, 255, 255, 0.9); font-weight: 500; }
      .eq-slider { -webkit-appearance: none; width: 100%; height: 3px; background: rgba(255, 255, 255, 0.1);
        border-radius: 2px; cursor: pointer;
      }
      .eq-slider::-webkit-slider-thumb { -webkit-appearance: none; width: 12px; height: 12px;
        background: #00e5ff; border-radius: 50%; cursor: pointer; box-shadow: 0 0 8px rgba(0, 229, 255, 0.5);
      }
      .eq-type-select { background: rgba(20, 25, 35, 0.9); border: 1px solid rgba(0, 229, 255, 0.2);
        color: rgba(255, 255, 255, 0.9); padding: 4px 6px; border-radius: 4px; font-size: 9px;
        cursor: pointer; font-family: inherit;
      }
    )CSS";

    inline const juce::String js = R"JS(
        document.addEventListener('selectstart', (e) => e.preventDefault());
        document.addEventListener('dragstart', (e) => e.preventDefault());
        document.addEventListener('contextmenu', (e) => e.preventDefault());
        
        const canvas = document.getElementById('glCanvas');
        const ctx = canvas ? canvas.getContext('2d') : null;
        function resizeCanvas() {
            if (!canvas || !ctx) return;
            const container = canvas.parentElement;
            const rect = container.getBoundingClientRect();
            const dpr = window.devicePixelRatio || 1;
            canvas.width = rect.width * dpr;
            canvas.height = rect.height * dpr;
            ctx.scale(dpr, dpr);
            canvas.style.width = rect.width + 'px';
            canvas.style.height = rect.height + 'px';
        }
        window.addEventListener('resize', resizeCanvas);
        resizeCanvas();
        
        class Particle {
            constructor(x, y, angle) {
                this.x = x; this.y = y; this.baseAngle = angle;
                this.speed = 0.3 + Math.random() * 0.5;
                this.offset = Math.random() * Math.PI * 2;
                this.size = 1.5 + Math.random() * 1.5;
                this.opacity = 0.6 + Math.random() * 0.4;
                this.hue = 180 + Math.random() * 60;
                this.vibrationPhase = Math.random() * Math.PI * 2;
            }
            update(centerX, centerY, time, audioIntensity, vibrationSpeed) {
                const idleSpeed = 0.2;
                const currentSpeed = idleSpeed + (vibrationSpeed - idleSpeed) * audioIntensity;
                this.vibrationPhase += currentSpeed * 0.05;
                const noise = Math.sin(this.vibrationPhase + this.offset) * 0.3;
                const angle = this.baseAngle + noise;
                const effectiveRadius = Math.min(canvas.width / (window.devicePixelRatio||1), canvas.height / (window.devicePixelRatio||1)) / 2;
                const baseDistance = Math.min(80, effectiveRadius * 0.35);
                const distance = baseDistance + Math.sin(time * 0.3 + this.offset) * 20 + audioIntensity * 60;
                this.x = centerX + Math.cos(angle) * distance;
                this.y = centerY + Math.sin(angle) * distance;
                this.baseAngle += 0.005 * currentSpeed * (this.offset > Math.PI ? 1 : -1);
            }
            draw(ctx) {
                const size = this.size * 0.8;
                ctx.fillStyle = 'hsla(' + this.hue + ', 80%, 60%, ' + this.opacity + ')';
                ctx.shadowBlur = 10;
                ctx.shadowColor = 'hsla(' + this.hue + ', 90%, 60%, 0.8)';
                ctx.fillRect(this.x - size/2, this.y - size/2, size, size);
                ctx.shadowBlur = 0;
            }
        }
        const numParticles = 500;
        const particles = [];
        for (let i = 0; i < numParticles; i++) {
            const angle = (i / numParticles) * Math.PI * 2;
            particles.push(new Particle(canvas ? canvas.width / 2 : 400, canvas ? canvas.height / 2 : 300, angle));
        }
        let activeNotes = 0, currentNotes = new Map(), audioIntensity = 0, vibrationSpeed = 0.5, time = 0;
        
        const baseKeyboardMap = {
            'KeyA': 0, 'KeyW': 1, 'KeyS': 2, 'KeyE': 3, 'KeyD': 4, 'KeyF': 5,
            'KeyT': 6, 'KeyG': 7, 'KeyY': 8, 'KeyH': 9, 'KeyU': 10, 'KeyJ': 11,
            'KeyK': 12, 'KeyO': 13, 'KeyL': 14, 'KeyP': 15, 'Semicolon': 16
        };
        let octaveOffset = 48;
        const pressedKeys = new Set();
        const activeKeyNotes = new Map();
        
        document.addEventListener('keydown', (e) => {
            if (e.repeat) return;
            if (e.code === 'KeyZ') { octaveOffset = Math.max(24, octaveOffset - 12); return; }
            if (e.code === 'KeyX') { octaveOffset = Math.min(84, octaveOffset + 12); return; }
            const noteOffset = baseKeyboardMap[e.code];
            if (noteOffset !== undefined && !pressedKeys.has(e.code)) {
                pressedKeys.add(e.code);
                const note = octaveOffset + noteOffset;
                activeKeyNotes.set(e.code, note);
                noteOn(note);
            }
        });
        document.addEventListener('keyup', (e) => {
            const noteOffset = baseKeyboardMap[e.code];
            if (noteOffset !== undefined && pressedKeys.has(e.code)) {
                pressedKeys.delete(e.code);
                const note = activeKeyNotes.get(e.code);
                if (note !== undefined) { activeKeyNotes.delete(e.code); noteOff(note); }
            }
        });
        
        const keyboard = document.getElementById('keyboard');
        if (keyboard) {
            const keys = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];
            for (let i = 48; i < 84; i++) {
                const key = document.createElement('div');
                const noteName = keys[i % 12];
                const isBlack = noteName.includes('#');
                key.className = 'key ' + (isBlack ? 'black' : 'white');
                key.dataset.note = i;
                key.addEventListener('mousedown', () => noteOn(i));
                key.addEventListener('mouseup', () => noteOff(i));
                key.addEventListener('mouseleave', () => { if (currentNotes.has(i)) noteOff(i); });
                keyboard.appendChild(key);
            }
        }
        function toggleTutorial() {
            const popups = ['popup-sphere', 'popup-controls', 'popup-keyboard'];
            const anyActive = popups.some(id => { const el = document.getElementById(id); return el && el.classList.contains('active'); });
            if (!anyActive) { const el = document.getElementById('popup-sphere'); if (el) el.classList.add('active'); }
            else popups.forEach(id => { const el = document.getElementById(id); if (el) el.classList.remove('active'); });
        }
        function dismissPopup(id) {
            const el = document.getElementById(id);
            if (el) el.classList.remove('active');
            if (id === 'popup-sphere') setTimeout(() => { const el = document.getElementById('popup-controls'); if (el) el.classList.add('active'); }, 200);
            else if (id === 'popup-controls') setTimeout(() => { const el = document.getElementById('popup-keyboard'); if (el) el.classList.add('active'); }, 200);
        }
        function updateVibrationSpeed() {
            if (currentNotes.size === 0) return;
            let totalFreq = 0;
            for (let n of currentNotes.keys()) totalFreq += 440 * Math.pow(2, (n - 69) / 12);
            const avgFreq = totalFreq / currentNotes.size;
            const normalized = Math.max(0, Math.min(1, (avgFreq - 130) / (1046 - 130)));
            vibrationSpeed = 0.5 + normalized * 7.5;
        }
        function toggleSettings() {
            const modal = document.getElementById('settings-modal');
            if (!modal) return;
            if (!modal.classList.contains('active')) { modal.classList.add('active'); window.location = 'sphere://getDevices'; }
            else modal.classList.remove('active');
        }
        function setAudioOutput(index) { window.location = 'sphere://setAudioOutput/' + index; }
        function setMidiInput(index) { window.location = 'sphere://setMidiInput/' + index; }
        function switchView(viewName) {
            closeEQ();
            document.querySelectorAll('.view-container').forEach(v => v.classList.remove('active'));
            const view = document.getElementById(viewName + '-view');
            if (view) view.classList.add('active');
            document.querySelectorAll('.view-btn').forEach(b => b.classList.remove('active'));
            const btn = document.querySelector('[data-view="' + viewName + '"]');
            if (btn) btn.classList.add('active');
            if (viewName === 'fx') initEQMiniCanvas();
        }
        function selectSound(type, btn) {
            document.querySelectorAll('.controls button').forEach(b => b.classList.remove('active'));
            if(btn) btn.classList.add('active');
            window.location = 'sphere://sound/' + type;
        }
        function updateMeter(level) {
            const bar = document.getElementById('volume-bar');
            if (!bar) return;
            const percentage = Math.min(100, Math.max(0, level * 100));
            bar.style.height = percentage + '%';
            bar.style.opacity = 0.5 + (level * 0.5);
        }
        function populateDevices(jsonString) {
            try {
                const data = JSON.parse(jsonString);
                const audioSelect = document.getElementById('audio-output');
                if (audioSelect) {
                    audioSelect.innerHTML = '';
                    data.audioOutputs.forEach((name, index) => {
                        const option = document.createElement('option');
                        option.text = name; option.value = index;
                        if (index === data.currentAudioOutput) option.selected = true;
                        audioSelect.add(option);
                    });
                }
                const midiSelect = document.getElementById('midi-input');
                if (midiSelect) {
                    midiSelect.innerHTML = '';
                    data.midiInputs.forEach((name, index) => {
                        const option = document.createElement('option');
                        option.text = name; option.value = index;
                        if (index === data.currentMidiInput) option.selected = true;
                        midiSelect.add(option);
                    });
                }
            } catch(e) { console.error("Error parsing device data", e); }
        }
        function noteOn(note) {
            if (currentNotes.has(note)) return;
            activeNotes++; currentNotes.set(note, Date.now()); updateVibrationSpeed();
            const key = document.querySelector('.key[data-note="' + note + '"]');
            if (key) key.classList.add('pressed');
            window.location = 'sphere://noteOn/' + note + '/100';
        }
        function noteOff(note) {
            if (!currentNotes.has(note)) return;
            activeNotes = Math.max(0, activeNotes - 1); currentNotes.delete(note); updateVibrationSpeed();
            const key = document.querySelector('.key[data-note="' + note + '"]');
            if (key) key.classList.remove('pressed');
            window.location = 'sphere://noteOff/' + note + '/0';
        }
        function visualizeNoteOn(note) {
            if (currentNotes.has(note)) return;
            activeNotes++; currentNotes.set(note, Date.now()); updateVibrationSpeed();
            const key = document.querySelector('.key[data-note="' + note + '"]');
            if (key) key.classList.add('pressed');
        }
        function visualizeNoteOff(note) {
            if (!currentNotes.has(note)) return;
            activeNotes = Math.max(0, activeNotes - 1); currentNotes.delete(note); updateVibrationSpeed();
            const key = document.querySelector('.key[data-note="' + note + '"]');
            if (key) key.classList.remove('pressed');
        }
        function render() {
            time += 0.016;
            const targetIntensity = activeNotes > 0 ? 1.0 : 0.0;
            audioIntensity += (targetIntensity - audioIntensity) * 0.1;
            if (ctx) {
                ctx.clearRect(0, 0, canvas.width, canvas.height);
                const dpr = window.devicePixelRatio || 1;
                const centerX = (canvas.width / dpr) / 2;
                const centerY = (canvas.height / dpr) / 2;
                particles.forEach(p => { p.update(centerX, centerY, time, audioIntensity, vibrationSpeed); p.draw(ctx); });
            }
            requestAnimationFrame(render);
        }
        render();
        let eqEnabled = true;
        const eqBands = [
            { active: true, type: 'lowshelf', freq: 100, gain: 2, q: 0.7 },
            { active: true, type: 'bell', freq: 2000, gain: 4, q: 1.0 },
            { active: false, type: 'bell', freq: 500, gain: 0, q: 1.0 },
            { active: false, type: 'bell', freq: 1000, gain: 0, q: 1.0 },
            { active: false, type: 'bell', freq: 4000, gain: 0, q: 1.0 },
            { active: false, type: 'highshelf', freq: 8000, gain: 0, q: 0.7 },
            { active: false, type: 'lowcut', freq: 30, gain: 0, q: 0.7 },
            { active: false, type: 'highcut', freq: 16000, gain: 0, q: 0.7 }
        ];
        const eqTypes = ['bell', 'lowshelf', 'highshelf', 'lowcut', 'highcut', 'notch'];
        function openEQ() { const panel = document.getElementById('eq-panel'); if (panel) { panel.classList.add('active'); initEQBands(); setTimeout(initEQCanvas, 50); } }
        function closeEQ() { const panel = document.getElementById('eq-panel'); if (panel) panel.classList.remove('active'); }
        function toggleEQMaster() { const toggle = document.getElementById('eq-master-toggle'); eqEnabled = !eqEnabled; if (toggle) toggle.classList.toggle('on', eqEnabled); window.location = 'sphere://eq/enable/' + (eqEnabled ? '1' : '0'); }
        function initEQBands() {
            const container = document.getElementById('eq-bands-container'); if (!container) return;
            container.innerHTML = '';
            eqBands.forEach((band, i) => {
                const el = document.createElement('div'); el.className = 'eq-band';
                el.innerHTML = '<div class="eq-band-header"><span class="eq-band-num">Band ' + (i+1) + '</span><div class="eq-band-toggle ' + (band.active ? 'on' : '') + '" onclick="toggleEQBand(' + i + ')"></div></div><div class="eq-param"><span class="eq-param-label">Type</span><select class="eq-type-select" onchange="setEQType(' + i + ', this.value)">' + eqTypes.map(t => '<option value="' + t + '"' + (t === band.type ? ' selected' : '') + '>' + t.toUpperCase() + '</option>').join('') + '</select></div><div class="eq-param"><span class="eq-param-label">Freq</span><span class="eq-param-value" id="freq-val-' + i + '">' + formatFreq(band.freq) + '</span><input type="range" class="eq-slider" min="20" max="20000" value="' + band.freq + '" oninput="setEQFreq(' + i + ', this.value)"></div><div class="eq-param"><span class="eq-param-label">Gain</span><span class="eq-param-value" id="gain-val-' + i + '">' + (band.gain > 0 ? '+' : '') + band.gain.toFixed(1) + ' dB</span><input type="range" class="eq-slider" min="-18" max="18" step="0.1" value="' + band.gain + '" oninput="setEQGain(' + i + ', this.value)"></div><div class="eq-param"><span class="eq-param-label">Q</span><span class="eq-param-value" id="q-val-' + i + '">' + band.q.toFixed(2) + '</span><input type="range" class="eq-slider" min="0.1" max="10" step="0.01" value="' + band.q + '" oninput="setEQQ(' + i + ', this.value)"></div>';
                container.appendChild(el);
            });
        }
        function formatFreq(f) { return f >= 1000 ? (f/1000).toFixed(1) + 'k' : Math.round(f) + ''; }
        function toggleEQBand(i) { eqBands[i].active = !eqBands[i].active; initEQBands(); sendEQUpdate(i); drawEQCurve(); }
        function setEQType(i, val) { eqBands[i].type = val; sendEQUpdate(i); drawEQCurve(); }
        function setEQFreq(i, val) { eqBands[i].freq = parseFloat(val); const el = document.getElementById('freq-val-' + i); if (el) el.textContent = formatFreq(eqBands[i].freq); sendEQUpdate(i); drawEQCurve(); }
        function setEQGain(i, val) { eqBands[i].gain = parseFloat(val); const el = document.getElementById('gain-val-' + i); if (el) el.textContent = (eqBands[i].gain > 0 ? '+' : '') + eqBands[i].gain.toFixed(1) + ' dB'; sendEQUpdate(i); drawEQCurve(); }
        function setEQQ(i, val) { eqBands[i].q = parseFloat(val); const el = document.getElementById('q-val-' + i); if (el) el.textContent = eqBands[i].q.toFixed(2); sendEQUpdate(i); drawEQCurve(); }
        function sendEQUpdate(i) { const b = eqBands[i]; window.location = 'sphere://eq/band/' + i + '/' + (b.active?1:0) + '/' + b.type + '/' + b.freq + '/' + b.gain + '/' + b.q; }
        let eqCanvas, eqCtx;
        function initEQCanvas() { eqCanvas = document.getElementById('eq-analyzer-canvas'); if (!eqCanvas) return; eqCtx = eqCanvas.getContext('2d'); const rect = eqCanvas.parentElement.getBoundingClientRect(); const dpr = window.devicePixelRatio || 1; eqCanvas.width = rect.width * dpr; eqCanvas.height = rect.height * dpr; eqCtx.scale(dpr, dpr); eqCanvas.style.width = rect.width + 'px'; eqCanvas.style.height = rect.height + 'px'; drawEQCurve(); }
        function drawEQCurve() { if (!eqCtx) return; const w = eqCanvas.width / (window.devicePixelRatio || 1); const h = eqCanvas.height / (window.devicePixelRatio || 1); eqCtx.clearRect(0, 0, w, h); eqCtx.strokeStyle = 'rgba(0, 229, 255, 0.2)'; eqCtx.lineWidth = 1; eqCtx.beginPath(); eqCtx.moveTo(0, h/2); eqCtx.lineTo(w, h/2); eqCtx.stroke(); eqCtx.strokeStyle = '#00e5ff'; eqCtx.lineWidth = 2; eqCtx.shadowBlur = 10; eqCtx.shadowColor = '#00e5ff'; eqCtx.beginPath(); for (let x = 0; x < w; x++) { const freq = 20 * Math.pow(1000, x / w); let totalGain = 0; eqBands.forEach(b => { if (!b.active) return; totalGain += calculateBandResponse(b, freq); }); const y = h/2 - (totalGain / 24) * (h/2); if (x === 0) eqCtx.moveTo(x, y); else eqCtx.lineTo(x, y); } eqCtx.stroke(); eqCtx.shadowBlur = 0; eqBands.forEach((b, i) => { if (!b.active) return; const x = w * Math.log10(b.freq / 20) / 3; const y = h/2 - (b.gain / 24) * (h/2); eqCtx.fillStyle = '#00e5ff'; eqCtx.beginPath(); eqCtx.arc(x, y, 6, 0, Math.PI * 2); eqCtx.fill(); eqCtx.fillStyle = '#000'; eqCtx.font = '10px sans-serif'; eqCtx.textAlign = 'center'; eqCtx.fillText(i+1, x, y + 3); }); }
        function calculateBandResponse(b, freq) { const ratio = freq / b.freq; const logRatio = Math.log2(ratio); switch(b.type) { case 'bell': return b.gain * Math.exp(-Math.pow(logRatio * b.q, 2) * 2); case 'lowshelf': return ratio < 1 ? b.gain : b.gain * Math.exp(-logRatio * 2); case 'highshelf': return ratio > 1 ? b.gain : b.gain * Math.exp(logRatio * 2); case 'lowcut': return ratio < 1 ? -24 * (1 - ratio) : 0; case 'highcut': return ratio > 1 ? -24 * (ratio - 1) : 0; case 'notch': return -12 * Math.exp(-Math.pow(logRatio * b.q * 3, 2)); default: return 0; } }
        function initEQMiniCanvas() { const miniCanvas = document.getElementById('eq-mini-canvas'); if (!miniCanvas) return; const miniCtx = miniCanvas.getContext('2d'); const rect = miniCanvas.getBoundingClientRect(); const dpr = window.devicePixelRatio || 1; miniCanvas.width = rect.width * dpr; miniCanvas.height = rect.height * dpr; miniCtx.scale(dpr, dpr); const w = rect.width, h = rect.height; miniCtx.clearRect(0, 0, w, h); miniCtx.strokeStyle = 'rgba(0, 229, 255, 0.3)'; miniCtx.lineWidth = 1; miniCtx.beginPath(); miniCtx.moveTo(0, h/2); miniCtx.lineTo(w, h/2); miniCtx.stroke(); miniCtx.strokeStyle = '#00e5ff'; miniCtx.lineWidth = 1.5; miniCtx.beginPath(); for (let x = 0; x < w; x++) { const freq = 20 * Math.pow(1000, x / w); let totalGain = 0; eqBands.forEach(b => { if (!b.active) return; totalGain += calculateBandResponse(b, freq); }); const y = h/2 - (totalGain / 24) * (h/2); if (x === 0) miniCtx.moveTo(x, y); else miniCtx.lineTo(x, y); } miniCtx.stroke(); }
    )JS";

    inline const juce::String html = R"HTML(<!DOCTYPE html><html><head><meta charset="UTF-8"><style>)HTML" + css + R"HTML(</style></head><body>
      <div class="top-nav">
        <button class="settings-btn" onclick="toggleSettings()">Settings</button>
        <div class="nav-center">
          <button class="view-btn active" data-view="synth" onclick="switchView('synth')">Synth</button>
          <button class="view-btn" data-view="fx" onclick="switchView('fx')">FX</button>
        </div>
        <button class="tutorial-btn" onclick="toggleTutorial()">? Tutorial</button>
      </div>
      <div class="footer-credit">Sphere DSP</div>
      <div id="synth-view" class="view-container active">
        <div class="header"><h1>SPHERE SYNTH</h1></div>
        <div class="sphere-container"><canvas id="glCanvas"></canvas></div>
        <div class="controls">
          <button onclick="selectSound('sine', this)" class="active">Sine</button>
          <button onclick="selectSound('saw', this)">Saw</button>
          <button onclick="selectSound('square', this)">Square</button>
          <button onclick="selectSound('sampled', this)">Cello</button>
        </div>
        <div class="keyboard-container"><div class="keyboard" id="keyboard"></div></div>
        <div class="volume-meter-container"><div class="volume-meter-bar" id="volume-bar"></div></div>
      </div>
      <div id="fx-view" class="view-container">
        <div class="fx-grid">
          <div class="fx-card" onclick="openEQ()">
            <div class="fx-card-header"><span class="fx-card-title">EQ</span><div class="fx-toggle on" id="eq-master-toggle" onclick="event.stopPropagation(); toggleEQMaster()"></div></div>
            <div class="fx-card-preview"><div class="eq-mini-display"><canvas class="eq-mini-canvas" id="eq-mini-canvas"></canvas></div></div>
          </div>
          <div class="fx-card" style="opacity: 0.4; pointer-events: none;"><div class="fx-card-header"><span class="fx-card-title">Reverb</span><div class="fx-toggle"></div></div><div class="fx-card-preview"><div style="color: rgba(255,255,255,0.3); font-size: 11px;">COMING SOON</div></div></div>
          <div class="fx-card" style="opacity: 0.4; pointer-events: none;"><div class="fx-card-header"><span class="fx-card-title">Delay</span><div class="fx-toggle"></div></div><div class="fx-card-preview"><div style="color: rgba(255,255,255,0.3); font-size: 11px;">COMING SOON</div></div></div>
          <div class="fx-card" style="opacity: 0.4; pointer-events: none;"><div class="fx-card-header"><span class="fx-card-title">Comp</span><div class="fx-toggle"></div></div><div class="fx-card-preview"><div style="color: rgba(255,255,255,0.3); font-size: 11px;">COMING SOON</div></div></div>
        </div>
      </div>
      <div id="eq-panel" class="eq-panel">
        <div class="eq-header"><span class="eq-title">Sphere EQ</span><button class="eq-close-btn" onclick="closeEQ()">&minus;</button></div>
        <div class="eq-display-container">
          <div class="eq-analyzer"><canvas id="eq-analyzer-canvas" class="eq-analyzer-canvas"></canvas><div class="eq-grid-overlay"></div><div class="eq-freq-labels"><span>20</span><span>50</span><span>100</span><span>200</span><span>500</span><span>1k</span><span>2k</span><span>5k</span><span>10k</span><span>20k</span></div><div class="eq-db-labels"><span>+12</span><span>+6</span><span>0</span><span>-6</span><span>-12</span></div></div>
          <div class="eq-bands-container" id="eq-bands-container"></div>
        </div>
      </div>
      <div id="popup-sphere" class="tutorial-popup"><div class="popup-title">Sphere Visualizer</div><div class="popup-text">This sphere reacts to your notes.</div><button class="popup-dismiss" onclick="dismissPopup('popup-sphere')">Next</button></div>
      <div id="popup-controls" class="tutorial-popup"><div class="popup-title">Sound Selection</div><div class="popup-text">Choose different waveforms.</div><button class="popup-dismiss" onclick="dismissPopup('popup-controls')">Next</button></div>
      <div id="popup-keyboard" class="tutorial-popup"><div class="popup-title">Piano Keyboard</div><div class="popup-text">Use A-J keys to play C3-B3, K-; for C4-E4. Press Z/X to shift octave down/up.</div><button class="popup-dismiss" onclick="dismissPopup('popup-keyboard')">Got It!</button></div>
      <div id="settings-modal" class="tutorial-popup" style="max-width: 400px; top: 50%; left: 50%; transform: translate(-50%, -50%);">
        <div class="popup-title">Settings</div>
        <div style="margin-bottom: 15px;"><label style="display: block; color: rgba(255,255,255,0.6); font-size: 11px; margin-bottom: 5px; text-transform: uppercase;">Audio Output</label><select id="audio-output" onchange="setAudioOutput(this.value)" class="settings-select"><option>Loading...</option></select></div>
        <div style="margin-bottom: 20px;"><label style="display: block; color: rgba(255,255,255,0.6); font-size: 11px; margin-bottom: 5px; text-transform: uppercase;">MIDI Input</label><select id="midi-input" onchange="setMidiInput(this.value)" class="settings-select"><option>Loading...</option></select></div>
        <button class="popup-dismiss" onclick="toggleSettings()">Close</button>
      </div>
    </body><script>)HTML" + js + R"HTML(</script></html>)HTML";
}
