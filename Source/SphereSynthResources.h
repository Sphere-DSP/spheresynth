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
      body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
        background: radial-gradient(ellipse at center, #0a0a1a 0%, #000005 50%, #000000 100%);
        min-height: 100vh; display: flex; flex-direction: column;
        align-items: center; overflow: hidden; -webkit-app-region: drag; position: relative;
        color: white; cursor: default;
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
      h1 { 
        font-size: clamp(24px, 5vw, 52px); 
        font-weight: 200; 
        letter-spacing: clamp(6px, 1.5vw, 14px);
        color: #ffffff; 
        text-shadow: 
          0 0 10px rgba(0, 229, 255, 0.8),
          0 0 20px rgba(0, 229, 255, 0.6),
          0 0 40px rgba(0, 229, 255, 0.4),
          0 0 80px rgba(0, 229, 255, 0.2);
        margin-bottom: 8px;
        animation: titleGlow 3s ease-in-out infinite;
      }
      @keyframes titleGlow {
        0%, 100% { 
          text-shadow: 
            0 0 10px rgba(0, 229, 255, 0.8),
            0 0 20px rgba(0, 229, 255, 0.6),
            0 0 40px rgba(0, 229, 255, 0.4),
            0 0 80px rgba(0, 229, 255, 0.2);
          color: #ffffff;
        }
        50% { 
          text-shadow: 
            0 0 20px rgba(0, 229, 255, 1),
            0 0 40px rgba(0, 229, 255, 0.8),
            0 0 60px rgba(100, 200, 255, 0.6),
            0 0 100px rgba(0, 229, 255, 0.4);
          color: #e0ffff;
        }
      }
      .sphere-container { position: fixed; top: 60px; left: 0; width: 100%; bottom: 180px;
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
    )CSS";

inline const juce::String css2 = R"CSS(
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
      /* ============================================
         SPHERE COMPRESSOR V2 - FabFilter Pro-C 2 Style
         ============================================ */
      .comp-panel { position: fixed; top: 60px; left: 0; right: 0; bottom: 0;
        background: #0b0d12; z-index: 1500;
        display: none; flex-direction: column; overflow: hidden;
      }
      .comp-panel.active { display: flex; }
      .comp-header { display: flex; justify-content: space-between; align-items: center;
        padding: 12px 24px; border-bottom: 1px solid rgba(0, 229, 255, 0.15); flex-shrink: 0;
        background: rgba(0, 0, 0, 0.4);
      }
      .comp-title { font-size: 14px; font-weight: 700; letter-spacing: 4px; color: #00e5ff;
        text-transform: uppercase; text-shadow: 0 0 20px rgba(0, 229, 255, 0.5);
      }
      .comp-header-btns { display: flex; gap: 8px; }
      .comp-reset-btn { background: rgba(255, 255, 255, 0.05); border: 1px solid rgba(255, 255, 255, 0.2);
        color: rgba(255, 255, 255, 0.7); padding: 6px 16px; border-radius: 4px; font-size: 10px;
        font-weight: 600; cursor: pointer; text-transform: uppercase; letter-spacing: 1px;
      }
      .comp-reset-btn:hover { border-color: rgba(0, 229, 255, 0.5); color: #00e5ff; }
      .comp-close-btn { background: rgba(255, 255, 255, 0.05); border: 1px solid rgba(255, 255, 255, 0.2);
        color: rgba(255, 255, 255, 0.8); width: 28px; height: 28px; border-radius: 4px;
        font-size: 18px; cursor: pointer; display: flex; align-items: center; justify-content: center; padding: 0;
      }
      .comp-close-btn:hover { background: rgba(255, 255, 255, 0.1); border-color: rgba(0, 229, 255, 0.5); color: #00e5ff; }
      
      /* Main visualizer area */
      .comp-main { flex: 1; display: flex; padding: 16px 24px; gap: 16px; min-height: 0; }
      .comp-visualizer { flex: 1; position: relative; background: rgba(0, 0, 0, 0.5);
        border: 1px solid rgba(0, 229, 255, 0.1); border-radius: 8px; overflow: hidden;
      }
      .comp-waveform-canvas { position: absolute; top: 0; left: 0; width: 100%; height: 100%; }
      .comp-transfer-canvas { position: absolute; top: 0; left: 0; width: 100%; height: 100%; cursor: crosshair; }
      .comp-grid-overlay { position: absolute; top: 0; left: 0; right: 0; bottom: 0; pointer-events: none;
        background-image: linear-gradient(rgba(255, 255, 255, 0.03) 1px, transparent 1px),
          linear-gradient(90deg, rgba(255, 255, 255, 0.03) 1px, transparent 1px);
        background-size: 40px 40px;
      }
      .comp-axis-x { position: absolute; bottom: 8px; left: 50px; right: 50px;
        display: flex; justify-content: space-between; font-size: 9px; color: rgba(255, 255, 255, 0.3);
        pointer-events: none;
      }
      .comp-axis-y { position: absolute; top: 20px; bottom: 30px; left: 10px;
        display: flex; flex-direction: column; justify-content: space-between; font-size: 9px;
        color: rgba(255, 255, 255, 0.3); pointer-events: none;
      }
      .comp-input-label { position: absolute; bottom: 4px; left: 50%; transform: translateX(-50%);
        font-size: 8px; color: rgba(0, 229, 255, 0.5); text-transform: uppercase; letter-spacing: 2px;
      }
      .comp-output-label { position: absolute; top: 50%; left: 4px; transform: rotate(-90deg) translateX(-50%);
        transform-origin: left center; font-size: 8px; color: rgba(0, 229, 255, 0.5);
        text-transform: uppercase; letter-spacing: 2px;
      }
      
      /* GR Meter - LED style */
      .comp-gr-meter { width: 50px; display: flex; flex-direction: column; align-items: center;
        background: rgba(0, 0, 0, 0.4); border: 1px solid rgba(0, 229, 255, 0.1); border-radius: 8px; padding: 12px 8px;
      }
      .comp-gr-label { font-size: 10px; color: rgba(255, 255, 255, 0.5); text-transform: uppercase;
        letter-spacing: 1px; margin-bottom: 8px;
      }
      .comp-gr-leds { flex: 1; width: 24px; display: flex; flex-direction: column-reverse; gap: 2px; }
      .comp-gr-led { width: 100%; height: 6px; background: rgba(255, 255, 255, 0.08); border-radius: 1px; }
      .comp-gr-led.active { background: linear-gradient(90deg, #ff6b35, #ffaa00); box-shadow: 0 0 8px rgba(255, 107, 53, 0.6); }
      .comp-gr-led.yellow { background: linear-gradient(90deg, #ffcc00, #ffee00); box-shadow: 0 0 8px rgba(255, 204, 0, 0.6); }
      .comp-gr-led.green { background: linear-gradient(90deg, #00ff88, #00ffaa); box-shadow: 0 0 8px rgba(0, 255, 136, 0.4); }
      .comp-gr-value { font-size: 10px; color: #ff6b35; margin-top: 8px; font-weight: 600; font-family: monospace; }
      
      /* Control Strip */
      .comp-control-strip { display: flex; justify-content: center; gap: 12px; padding: 16px 24px;
        background: rgba(0, 0, 0, 0.3); border-top: 1px solid rgba(0, 229, 255, 0.1);
      }
      .comp-knob-group { display: flex; flex-direction: column; align-items: center; gap: 6px;
        background: rgba(15, 20, 30, 0.6); border: 1px solid rgba(0, 229, 255, 0.1);
        border-radius: 8px; padding: 12px 16px; min-width: 90px;
      }
      .comp-knob-group:hover { border-color: rgba(0, 229, 255, 0.3); }
      .comp-knob-label { font-size: 9px; color: rgba(255, 255, 255, 0.4); text-transform: uppercase;
        letter-spacing: 1px;
      }
      .comp-knob-value { font-size: 14px; font-weight: 500; color: #00e5ff; font-family: monospace; }
      .comp-knob-slider { -webkit-appearance: none; width: 70px; height: 4px;
        background: rgba(255, 255, 255, 0.1); border-radius: 2px; cursor: pointer;
      }
      .comp-knob-slider::-webkit-slider-thumb { -webkit-appearance: none; width: 14px; height: 14px;
        background: linear-gradient(135deg, #00e5ff, #00b8d4); border-radius: 50%; cursor: pointer;
        box-shadow: 0 0 10px rgba(0, 229, 255, 0.6), 0 2px 4px rgba(0, 0, 0, 0.3);
      }
      .comp-knob-slider::-webkit-slider-thumb:hover { box-shadow: 0 0 15px rgba(0, 229, 255, 0.8); }
      
      /* Threshold point indicator */
      .comp-threshold-indicator { position: absolute; width: 16px; height: 16px;
        background: radial-gradient(circle, #00e5ff, #0088aa); border: 2px solid #fff;
        border-radius: 50%; cursor: grab; transform: translate(-50%, -50%);
        box-shadow: 0 0 20px rgba(0, 229, 255, 0.8), 0 0 40px rgba(0, 229, 255, 0.4);
        z-index: 10;
      }
      .comp-threshold-indicator:active { cursor: grabbing; }
      .comp-threshold-indicator::after { content: ''; position: absolute; top: 50%; left: 50%;
        width: 6px; height: 6px; background: #fff; border-radius: 50%; transform: translate(-50%, -50%);
      }
    )CSS";

inline const juce::String jsPart1 = R"JS(
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
            ctx.setTransform(1, 0, 0, 1, 0, 0);
            ctx.scale(dpr, dpr);
            canvas.style.width = rect.width + 'px';
            canvas.style.height = rect.height + 'px';
        }
        window.addEventListener('resize', resizeCanvas);
        resizeCanvas();
        

        class Particle {
            constructor(x, y, angle, ring) {
                this.x = x; this.y = y; this.baseAngle = angle;
                this.ring = ring; // 0, 1, or 2 for different orbital rings
                this.speed = 0.3 + Math.random() * 0.5;
                this.offset = Math.random() * Math.PI * 2;
                this.size = 1 + Math.random() * 2;
                this.opacity = 0.5 + Math.random() * 0.5;
                this.baseHue = 180 + ring * 30; // Different hue per ring
                this.hue = this.baseHue;
                this.vibrationPhase = Math.random() * Math.PI * 2;
                this.trail = [];
                this.trailLength = 5 + Math.floor(Math.random() * 5);
                this.pulsePhase = Math.random() * Math.PI * 2;
                this.orbitDirection = ring % 2 === 0 ? 1 : -1;
            }
            update(centerX, centerY, time, audioIntensity, vibrationSpeed) {
                // Store trail
                if (this.trail.length > this.trailLength) this.trail.shift();
                this.trail.push({x: this.x, y: this.y});
                
                const idleSpeed = 0.15;
                const currentSpeed = idleSpeed + (vibrationSpeed - idleSpeed) * audioIntensity;
                this.vibrationPhase += currentSpeed * 0.05;
                
                // Dynamic color shift based on audio
                this.hue = this.baseHue + Math.sin(time * 0.5 + this.offset) * 20 + audioIntensity * 40;
                
                // Pulsing size
                this.pulsePhase += 0.03;
                const pulseFactor = 1 + Math.sin(this.pulsePhase) * 0.3 * (1 + audioIntensity);
                
                const noise = Math.sin(this.vibrationPhase + this.offset) * 0.4;
                const angle = this.baseAngle + noise;
                const effectiveRadius = Math.min(canvas.width / (window.devicePixelRatio||1), canvas.height / (window.devicePixelRatio||1)) / 2;
                
                // Different radius per ring
                const ringRadius = [0.3, 0.45, 0.6][this.ring];
                const baseDistance = effectiveRadius * ringRadius;
                const waveOffset = Math.sin(time * 0.3 + this.offset + this.ring) * 15;
                const audioExpand = audioIntensity * 80;
                const distance = baseDistance + waveOffset + audioExpand;
                
                this.x = centerX + Math.cos(angle) * distance;
                this.y = centerY + Math.sin(angle) * distance;
                this.baseAngle += 0.003 * currentSpeed * this.orbitDirection * (1 + this.ring * 0.3);
                this.currentSize = this.size * pulseFactor;
            }
            draw(ctx, audioIntensity) {
                // Draw trail
                for (let i = 0; i < this.trail.length; i++) {
                    const t = this.trail[i];
                    const alpha = (i / this.trail.length) * 0.3 * this.opacity;
                    const size = this.currentSize * (i / this.trail.length) * 0.5;
                    ctx.fillStyle = 'hsla(' + this.hue + ', 80%, 60%, ' + alpha + ')';
                    ctx.fillRect(t.x - size/2, t.y - size/2, size, size);
                }
                
                // Draw main particle with glow
                const glowIntensity = 10 + audioIntensity * 20;
                ctx.shadowBlur = glowIntensity;
                ctx.shadowColor = 'hsla(' + this.hue + ', 100%, 60%, 0.9)';
                ctx.fillStyle = 'hsla(' + this.hue + ', 85%, 65%, ' + this.opacity + ')';
                ctx.beginPath();
                ctx.arc(this.x, this.y, this.currentSize, 0, Math.PI * 2);
                ctx.fill();
                ctx.shadowBlur = 0;
            }
        }
        
        // Create particles in 3 orbital rings
        const numParticles = 600;
        const particles = [];
        for (let i = 0; i < numParticles; i++) {
            const ring = i % 3;
            const angle = (i / numParticles) * Math.PI * 2 * 3; // Spiral distribution
            particles.push(new Particle(canvas ? canvas.width / 2 : 400, canvas ? canvas.height / 2 : 300, angle, ring));
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
    )JS";

inline const juce::String jsPart2 = R"JS(
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
            closeComp();
            document.querySelectorAll('.view-container').forEach(v => v.classList.remove('active'));
            const view = document.getElementById(viewName + '-view');
            if (view) view.classList.add('active');
            document.querySelectorAll('.view-btn').forEach(b => b.classList.remove('active'));
            const btn = document.querySelector('[data-view="' + viewName + '"]');
            if (btn) btn.classList.add('active');
            if (viewName === 'fx') initEQMiniCanvas();
            if (viewName === 'synth') setTimeout(resizeCanvas, 50);
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
                
                // Draw particles (sphere)
                particles.forEach(p => { p.update(centerX, centerY, time, audioIntensity, vibrationSpeed); p.draw(ctx, audioIntensity); });
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

inline const juce::String jsPart3 = R"JS(
        // ============================================
        // SPHERE COMPRESSOR V2 - FabFilter Pro-C 2 Style
        // ============================================
        let compEnabled = true;
        let compSettings = { threshold: -20, ratio: 4, attack: 10, release: 100, makeup: 0, knee: 6 };
        let compTransferCanvas, compTransferCtx, compWaveformCanvas, compWaveformCtx;
        let compDragging = false;
        
        // Create window globals for C++ to push data
        window.waveformBuffer = window.waveformBuffer || [];
        window.grBuffer = window.grBuffer || [];
        
        // Local references point to the same arrays
        let waveformBuffer = window.waveformBuffer;
        let grBuffer = window.grBuffer;
        const WAVEFORM_LENGTH = 200;
        const NUM_LEDS = 20;
        
        // Initialize buffers with zeros if empty
        while (waveformBuffer.length < WAVEFORM_LENGTH) waveformBuffer.push(0);
        while (grBuffer.length < WAVEFORM_LENGTH) grBuffer.push(0);
        
        function openComp() {
            const panel = document.getElementById('comp-panel');
            if (panel) {
                panel.classList.add('active');
                setTimeout(initCompressorUI, 50);
            }
        }
        
        function closeComp() {
            const panel = document.getElementById('comp-panel');
            if (panel) panel.classList.remove('active');
        }
        
        function toggleCompMaster() {
            const toggle = document.getElementById('comp-master-toggle');
            compEnabled = !compEnabled;
            if (toggle) toggle.classList.toggle('on', compEnabled);
            window.location = 'sphere://comp/enable/' + (compEnabled ? '1' : '0');
        }
        
        function initCompCanvases() {
            // Transfer curve canvas
            compTransferCanvas = document.getElementById('comp-transfer-canvas');
            if (compTransferCanvas) {
                compTransferCtx = compTransferCanvas.getContext('2d');
                resizeCompCanvas(compTransferCanvas, compTransferCtx);
                
                // Add mouse event listeners for dragging
                compTransferCanvas.addEventListener('mousedown', onCompMouseDown);
                compTransferCanvas.addEventListener('mousemove', onCompMouseMove);
                compTransferCanvas.addEventListener('mouseup', onCompMouseUp);
                compTransferCanvas.addEventListener('mouseleave', onCompMouseUp);
            }
            
            // Waveform canvas
            compWaveformCanvas = document.getElementById('comp-waveform-canvas');
            if (compWaveformCanvas) {
                compWaveformCtx = compWaveformCanvas.getContext('2d');
                resizeCompCanvas(compWaveformCanvas, compWaveformCtx);
            }
            
            drawTransferCurve();
            animateWaveform();
        }
        
        function resizeAllCompCanvases() {
            const panel = document.getElementById('comp-panel');
            if (!panel || !panel.classList.contains('active')) return;
            if (compTransferCanvas && compTransferCtx) {
                resizeCompCanvas(compTransferCanvas, compTransferCtx);
                drawTransferCurve();
            }
            if (compWaveformCanvas && compWaveformCtx) {
                resizeCompCanvas(compWaveformCanvas, compWaveformCtx);
            }
        }
        
        window.addEventListener('resize', resizeAllCompCanvases);
        
        function resizeCompCanvas(canvas, ctx) {
            if (!canvas || !ctx) return;
            const rect = canvas.parentElement.getBoundingClientRect();
            if (rect.width === 0 || rect.height === 0) return;
            const dpr = window.devicePixelRatio || 1;
            canvas.width = rect.width * dpr;
            canvas.height = rect.height * dpr;
            ctx.setTransform(1, 0, 0, 1, 0, 0); // Reset transform
            ctx.scale(dpr, dpr);
            canvas.style.width = rect.width + 'px';
            canvas.style.height = rect.height + 'px';
        }
        
        function drawTransferCurve() {
            if (!compTransferCtx || !compTransferCanvas) return;
            
            // Get actual canvas dimensions
            const canvasW = compTransferCanvas.width;
            const canvasH = compTransferCanvas.height;
            const dpr = window.devicePixelRatio || 1;
            const w = canvasW / dpr;
            const h = canvasH / dpr;
            const padding = 40;
            const graphW = w - padding * 2;
            const graphH = h - padding * 2;
            
            // Clear entire canvas (raw pixels)
            compTransferCtx.save();
            compTransferCtx.setTransform(1, 0, 0, 1, 0, 0);
            compTransferCtx.clearRect(0, 0, canvasW, canvasH);
            compTransferCtx.restore();
            
            // Draw transfer curve
            compTransferCtx.strokeStyle = '#00e5ff';
            compTransferCtx.lineWidth = 2.5;
            compTransferCtx.shadowBlur = 15;
            compTransferCtx.shadowColor = '#00e5ff';
            compTransferCtx.beginPath();
            
            for (let i = 0; i <= graphW; i++) {
                const inputDb = -60 + (i / graphW) * 60;
                const outputDb = calculateOutputDb(inputDb);
                const x = padding + i;
                const y = padding + ((0 - outputDb) / 60) * graphH;
                if (i === 0) compTransferCtx.moveTo(x, y);
                else compTransferCtx.lineTo(x, y);
            }
            compTransferCtx.stroke();
            compTransferCtx.shadowBlur = 0;
            
            // Draw threshold point
            const threshX = padding + ((compSettings.threshold + 60) / 60) * graphW;
            const threshY = padding + ((0 - compSettings.threshold) / 60) * graphH;
            
            // Glow effect
            const gradient = compTransferCtx.createRadialGradient(threshX, threshY, 0, threshX, threshY, 25);
            gradient.addColorStop(0, 'rgba(0, 229, 255, 0.4)');
            gradient.addColorStop(1, 'rgba(0, 229, 255, 0)');
            compTransferCtx.fillStyle = gradient;
            compTransferCtx.fillRect(threshX - 25, threshY - 25, 50, 50);
            
            // Point
            compTransferCtx.beginPath();
            compTransferCtx.arc(threshX, threshY, 8, 0, Math.PI * 2);
            compTransferCtx.fillStyle = '#00e5ff';
            compTransferCtx.fill();
            compTransferCtx.strokeStyle = '#fff';
            compTransferCtx.lineWidth = 2;
            compTransferCtx.stroke();
            
            // Inner dot
            compTransferCtx.beginPath();
            compTransferCtx.arc(threshX, threshY, 3, 0, Math.PI * 2);
            compTransferCtx.fillStyle = '#fff';
            compTransferCtx.fill();
            
            // Draw axis labels
            compTransferCtx.fillStyle = 'rgba(255, 255, 255, 0.4)';
            compTransferCtx.font = '9px -apple-system, sans-serif';
            compTransferCtx.textAlign = 'center';
            for (let db = -60; db <= 0; db += 20) {
                const x = padding + ((db + 60) / 60) * graphW;
                compTransferCtx.fillText(db + '', x, h - padding + 15);
            }
            compTransferCtx.textAlign = 'right';
            for (let db = -60; db <= 0; db += 20) {
                const y = padding + ((0 - db) / 60) * graphH;
                compTransferCtx.fillText(db + '', padding - 8, y + 3);
            }
        }
        
        function calculateOutputDb(inputDb) {
            const thresh = compSettings.threshold;
            const ratio = compSettings.ratio;
            const knee = compSettings.knee;
            
            if (inputDb < thresh - knee / 2) {
                return inputDb;
            }
            
            if (knee > 0 && inputDb < thresh + knee / 2) {
                // Soft knee region
                const x = inputDb - thresh + knee / 2;
                const overThreshold = (x * x) / (2 * knee);
                return inputDb - overThreshold * (1 - 1 / ratio);
            }
            
            // Above threshold
            const overThreshold = inputDb - thresh;
            return thresh + overThreshold / ratio;
        }
        
        // Mouse handlers for threshold point dragging
        function onCompMouseDown(e) {
            if (!compTransferCanvas) return;
            const rect = compTransferCanvas.getBoundingClientRect();
            const dpr = window.devicePixelRatio || 1;
            const w = compTransferCanvas.width / dpr;
            const h = compTransferCanvas.height / dpr;
            const padding = 40;
            const graphW = w - padding * 2;
            const graphH = h - padding * 2;
            
            const mouseX = e.clientX - rect.left;
            const mouseY = e.clientY - rect.top;
            
            // Calculate threshold point position
            const threshX = padding + ((compSettings.threshold + 60) / 60) * graphW;
            const threshY = padding + ((0 - compSettings.threshold) / 60) * graphH;
            
            // Check if click is near threshold point (within 15px)
            const dist = Math.sqrt((mouseX - threshX) ** 2 + (mouseY - threshY) ** 2);
            if (dist < 15) {
                compDragging = true;
                compTransferCanvas.style.cursor = 'grabbing';
            }
        }
        
        function onCompMouseMove(e) {
            if (!compTransferCanvas) return;
            const rect = compTransferCanvas.getBoundingClientRect();
            const dpr = window.devicePixelRatio || 1;
            const w = compTransferCanvas.width / dpr;
            const h = compTransferCanvas.height / dpr;
            const padding = 40;
            const graphW = w - padding * 2;
            const graphH = h - padding * 2;
            
            const mouseX = e.clientX - rect.left;
            const mouseY = e.clientY - rect.top;
            
            if (compDragging) {
                // Convert mouse position to threshold dB value
                const xNorm = (mouseX - padding) / graphW;
                const newThreshold = Math.max(-60, Math.min(0, -60 + xNorm * 60));
                
                compSettings.threshold = Math.round(newThreshold * 10) / 10;
                if (compKnobs && compKnobs['threshold']) {
                    compKnobs['threshold'].setValue(compSettings.threshold);
                }
                window.location = 'sphere://comp/threshold/' + compSettings.threshold;
                drawTransferCurve();
            } else {
                // Check if hovering over threshold point
                const threshX = padding + ((compSettings.threshold + 60) / 60) * graphW;
                const threshY = padding + ((0 - compSettings.threshold) / 60) * graphH;
                const dist = Math.sqrt((mouseX - threshX) ** 2 + (mouseY - threshY) ** 2);
                
                if (dist < 15) {
                    compTransferCanvas.style.cursor = 'grab';
                } else {
                    compTransferCanvas.style.cursor = 'default';
                }
            }
        }
        
        function onCompMouseUp(e) {
            compDragging = false;
            if (compTransferCanvas) {
                compTransferCanvas.style.cursor = 'default';
            }
        }
    )JS";

inline const juce::String jsPart4 = R"JS(
class Knob {
    constructor(container, label, options = {}) {
        this.container = typeof container === 'string' ? document.getElementById(container) : container;
        this.label = label;
        this.min = options.min !== undefined ? options.min : 0;
        this.max = options.max !== undefined ? options.max : 100;
        this.value = options.value !== undefined ? options.value : this.min;
        this.step = options.step !== undefined ? options.step : 1;
        this.units = options.units || '';
        this.size = options.size || 60;
        this.onChange = options.onChange || (() => {});
        this.color = options.color || '#00e5ff';
        this.render(); this.attachEvents();
    }
    render() {
        if (!this.container) return;
        const center = this.size / 2;
        const radius = this.size * 0.4;
        const strokeWidth = this.size * 0.08;
        const startAngle = -Math.PI * 0.75;
        const endAngle = Math.PI * 0.75;
        const rangeAngle = endAngle - startAngle;
        const percentage = (this.value - this.min) / (this.max - this.min);
        const currentAngle = startAngle + (percentage * rangeAngle);
        const bgParams = this.describeArc(center, center, radius, startAngle, endAngle);
        const valParams = this.describeArc(center, center, radius, startAngle, currentAngle);
        const capRadius = radius * 0.75;
        const markerX = center + (capRadius - 5) * Math.cos(currentAngle);
        const markerY = center + (capRadius - 5) * Math.sin(currentAngle);
        const markerStartX = center + (capRadius * 0.2) * Math.cos(currentAngle);
        const markerStartY = center + (capRadius * 0.2) * Math.sin(currentAngle);
        this.container.innerHTML = `<div class="knob-wrapper" style="display: flex; flex-direction: column; align-items: center; gap: 8px; pointer-events: none;"><div class="knob-circle" style="position: relative; width: ${this.size}px; height: ${this.size}px; pointer-events: auto; cursor: ns-resize;"><svg width="${this.size}" height="${this.size}" viewBox="0 0 ${this.size} ${this.size}"><defs><linearGradient id="knobGradient" x1="0%" y1="0%" x2="0%" y2="100%"><stop offset="0%" style="stop-color:#3a3a40;stop-opacity:1" /><stop offset="100%" style="stop-color:#2a2a30;stop-opacity:1" /></linearGradient><filter id="dropshadow" height="130%"><feGaussianBlur in="SourceAlpha" stdDeviation="2"/><feOffset dx="0" dy="2" result="offsetblur"/><feFlood flood-color="rgba(0,0,0,0.5)"/><feComposite in2="offsetblur" operator="in"/><feMerge><feMergeNode/><feMergeNode in="SourceGraphic"/></feMerge></filter></defs><path d="${bgParams}" fill="none" stroke="#1a1a1f" stroke-width="${strokeWidth}" stroke-linecap="round"/><path d="${valParams}" fill="none" stroke="${this.color}" stroke-width="${strokeWidth}" stroke-linecap="round" style="filter: drop-shadow(0 0 3px ${this.color}); opacity: 0.8;" /><circle cx="${center}" cy="${center}" r="${capRadius}" fill="url(#knobGradient)" stroke="#111" stroke-width="1" filter="url(#dropshadow)" /><line x1="${markerStartX}" y1="${markerStartY}" x2="${markerX}" y2="${markerY}" stroke="${this.color}" stroke-width="2" stroke-linecap="round" /></svg></div><div class="knob-info" style="text-align: center; line-height: 1.2;"><div class="knob-value" style="font-family: 'Segoe UI', sans-serif; font-size: 11px; color: ${this.color}; font-weight: 600;">${this.formatValue(this.value)} ${this.units}</div><div class="knob-label" style="font-size: 10px; text-transform: uppercase; color: #888; font-weight: 500; letter-spacing: 0.5px;">${this.label}</div></div></div>`;
        this.knobElement = this.container.querySelector('.knob-circle');
    }
    describeArc(x, y, radius, startAngle, endAngle) {
        const start = this.polarToCartesian(x, y, radius, endAngle);
        const end = this.polarToCartesian(x, y, radius, startAngle);
        const largeArcFlag = endAngle - startAngle <= Math.PI ? "0" : "1";
        return ["M", start.x, start.y, "A", radius, radius, 0, largeArcFlag, 0, end.x, end.y].join(" ");
    }
    polarToCartesian(centerX, centerY, radius, angleInRadians) {
        return { x: centerX + (radius * Math.cos(angleInRadians)), y: centerY + (radius * Math.sin(angleInRadians)) };
    }
    attachEvents() {
        if (!this.knobElement) return;
        this.knobElement.addEventListener('mousedown', (e) => {
            this.isDragging = true; this.lastY = e.clientY; document.body.style.cursor = 'ns-resize';
            window.addEventListener('mousemove', this.handleMouseMove); window.addEventListener('mouseup', this.handleMouseUp);
        });
    }
    handleMouseMove = (e) => {
        if (!this.isDragging) return;
        const deltaY = this.lastY - e.clientY; this.lastY = e.clientY;
        const range = this.max - this.min; const sensitivity = 200; 
        let newValue = this.value + (deltaY / sensitivity) * range; newValue = Math.max(this.min, Math.min(this.max, newValue));
        if (newValue !== this.value) { this.setValue(newValue); this.onChange(this.value); }
    }
    handleMouseUp = () => {
        this.isDragging = false; document.body.style.cursor = '';
        window.removeEventListener('mousemove', this.handleMouseMove); window.removeEventListener('mouseup', this.handleMouseUp);
    }
    setValue(val) { this.value = val; this.render(); this.attachEvents(); }
    formatValue(val) { if (this.step < 1) return val.toFixed(1); if (Math.abs(val) < 10 && this.step % 1 !== 0) return val.toFixed(1); return Math.round(val); }
}

let compKnobs = {};
let compDeltaEnabled = false;

// Globals already set up in jsPart3

function initCompressorUI() {
    console.log("Initializing Pro Compressor UI...");
    const controlStrip = document.getElementById('comp-controls');
    if (!controlStrip) return;
    controlStrip.innerHTML = '';
    controlStrip.className = 'comp-pro-controls';
    
    // 1. Threshold
    const threshSection = document.createElement('div');
    threshSection.className = 'comp-section comp-section-thresh';
    controlStrip.appendChild(threshSection);
    const threshWrapper = document.createElement('div');
    threshWrapper.id = 'knob-container-threshold';
    threshSection.appendChild(threshWrapper);
    compKnobs['threshold'] = new Knob(threshWrapper, 'Threshold', {
        min: -60, max: 0, value: compSettings.threshold, step: 0.1, units: 'dB', size: 90, color: '#00e5ff',
        onChange: (val) => { compSettings.threshold = val; window.location = `sphere://comp/threshold/${val}`; drawTransferCurve(); }
    });
    
    // 2. Main
    const mainSection = document.createElement('div');
    mainSection.className = 'comp-section comp-section-main';
    controlStrip.appendChild(mainSection);
    const mainParams = [
        { id: 'ratio', label: 'Ratio', min: 1, max: 20, val: compSettings.ratio, step: 0.1, units: ':1', size: 65 },
        { id: 'attack', label: 'Attack', min: 0.1, max: 500, val: compSettings.attack, step: 0.1, units: 'ms', size: 65 },
        { id: 'release', label: 'Release', min: 10, max: 3000, val: compSettings.release, step: 1, units: 'ms', size: 65 }
    ];
    mainParams.forEach(p => {
        const w = document.createElement('div'); w.id = `knob-container-${p.id}`; mainSection.appendChild(w);
        compKnobs[p.id] = new Knob(w, p.label, {
            min: p.min, max: p.max, value: p.val, step: p.step, units: p.units, size: p.size, color: '#ffb74d',
            onChange: (val) => { compSettings[p.id] = val; window.location = `sphere://comp/${p.id}/${val}`; if (p.id === 'ratio') drawTransferCurve(); }
        });
    });
    
    // 3. Secondary
    const secSection = document.createElement('div');
    secSection.className = 'comp-section comp-section-sec';
    controlStrip.appendChild(secSection);
    const secParams = [
        { id: 'knee', label: 'Knee', min: 0, max: 24, val: compSettings.knee, step: 0.1, units: 'dB', size: 50 },
        { id: 'makeup', label: 'Gain', min: -12, max: 24, val: compSettings.makeup, step: 0.1, units: 'dB', size: 50 }
    ];
    secParams.forEach(p => {
        const w = document.createElement('div'); w.id = `knob-container-${p.id}`; secSection.appendChild(w);
        compKnobs[p.id] = new Knob(w, p.label, {
            min: p.min, max: p.max, value: p.val, step: p.step, units: p.units, size: p.size, color: '#aaa',
            onChange: (val) => { compSettings[p.id] = val; window.location = `sphere://comp/${p.id}/${val}`; if (p.id === 'knee') drawTransferCurve(); }
        });
    });

     const deltaWrapper = document.createElement('div');
     deltaWrapper.className = 'comp-delta-wrapper';
     deltaWrapper.innerHTML = `<div class="comp-delta-btn ${compDeltaEnabled ? 'active' : ''}" onclick="toggleDelta()"><svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M3 18v-6a9 9 0 0 1 18 0v6"></path><path d="M21 19a2 2 0 0 1-2 2h-1a2 2 0 0 1-2 2h-1a2 2 0 0 1-2-2v-3a2 2 0 0 1 2-2h3zM3 19a2 2 0 0 0 2 2h1a2 2 0 0 0 2-2v-3a2 2 0 0 0-2-2H3z"></path></svg></div><div class="knob-label">DELTA</div>`;
     secSection.appendChild(deltaWrapper);
    
    if (typeof initCompCanvases === 'function') initCompCanvases();
}

function toggleDelta() {
    compDeltaEnabled = !compDeltaEnabled;
    const btn = document.querySelector('.comp-delta-btn');
    if (btn) btn.classList.toggle('active', compDeltaEnabled);
    window.location = `sphere://comp/delta/${compDeltaEnabled ? 1 : 0}`;
}

function updateKnob(param, value) {
    if (compKnobs[param]) compKnobs[param].setValue(value);
}

function animateWaveform() { drawWaveform(); requestAnimationFrame(animateWaveform); }

function drawWaveform() {
    if (!compWaveformCtx || !compWaveformCanvas) return;
    
    const canvasW = compWaveformCanvas.width;
    const canvasH = compWaveformCanvas.height;
    const dpr = window.devicePixelRatio || 1;
    const w = canvasW / dpr;
    const h = canvasH / dpr;
    
    // Clear canvas properly
    compWaveformCtx.save();
    compWaveformCtx.setTransform(1, 0, 0, 1, 0, 0);
    compWaveformCtx.clearRect(0, 0, canvasW, canvasH);
    compWaveformCtx.restore();
    
    const wf = window.waveformBuffer || [];
    const gr = window.grBuffer || [];
    
    // Layer 1: Input Signal (Gray filled area from bottom)
    if (wf.length > 0) {
        compWaveformCtx.beginPath();
        compWaveformCtx.moveTo(0, h);
        for (let i = 0; i < wf.length; i++) {
            const x = (i / wf.length) * w;
            const level = wf[i] || 0;
            // Convert to dB, map to height
            const db = level > 0.0001 ? 20 * Math.log10(level) : -60;
            const normalized = (Math.max(-60, db) + 60) / 60;
            const y = h - (normalized * h * 0.9); // 90% of height
            compWaveformCtx.lineTo(x, y);
        }
        compWaveformCtx.lineTo(w, h);
        compWaveformCtx.closePath();
        
        // Semi-transparent gray fill
        const inputGrad = compWaveformCtx.createLinearGradient(0, 0, 0, h);
        inputGrad.addColorStop(0, 'rgba(100, 100, 100, 0.3)');
        inputGrad.addColorStop(1, 'rgba(50, 50, 50, 0.1)');
        compWaveformCtx.fillStyle = inputGrad;
        compWaveformCtx.fill();
        
        // Thin white line on top
        compWaveformCtx.strokeStyle = 'rgba(255, 255, 255, 0.5)';
        compWaveformCtx.lineWidth = 1;
        compWaveformCtx.beginPath();
        for (let i = 0; i < wf.length; i++) {
            const x = (i / wf.length) * w;
            const level = wf[i] || 0;
            const db = level > 0.0001 ? 20 * Math.log10(level) : -60;
            const normalized = (Math.max(-60, db) + 60) / 60;
            const y = h - (normalized * h * 0.9);
            if (i === 0) compWaveformCtx.moveTo(x, y);
            else compWaveformCtx.lineTo(x, y);
        }
        compWaveformCtx.stroke();
    }
    
    // Layer 2: Gain Reduction (Red line/area from top)
    if (gr.length > 0) {
        compWaveformCtx.beginPath();
        compWaveformCtx.moveTo(0, 0);
        for (let i = 0; i < gr.length; i++) {
            const x = (i / gr.length) * w;
            const grVal = gr[i] || 0;
            // GR is in dB (0-24dB range typically)
            const y = (grVal / 24) * h * 0.5; // Max 50% of height
            compWaveformCtx.lineTo(x, y);
        }
        compWaveformCtx.lineTo(w, 0);
        compWaveformCtx.closePath();
        
        // Red gradient fill from top
        const grGrad = compWaveformCtx.createLinearGradient(0, 0, 0, h * 0.5);
        grGrad.addColorStop(0, 'rgba(255, 60, 60, 0.6)');
        grGrad.addColorStop(1, 'rgba(255, 60, 60, 0)');
        compWaveformCtx.fillStyle = grGrad;
        compWaveformCtx.fill();
        
        // Red line
        compWaveformCtx.strokeStyle = '#ff3d3d';
        compWaveformCtx.lineWidth = 2;
        compWaveformCtx.beginPath();
        for (let i = 0; i < gr.length; i++) {
            const x = (i / gr.length) * w;
            const grVal = gr[i] || 0;
            const y = (grVal / 24) * h * 0.5;
            if (i === 0) compWaveformCtx.moveTo(x, y);
            else compWaveformCtx.lineTo(x, y);
        }
        compWaveformCtx.stroke();
    }
}

function updateCompMeter(grDb) {
    const leds = document.querySelectorAll('.comp-gr-led');
    const activeLeds = Math.min(20, Math.floor((grDb / 24) * 20));
    leds.forEach((led, i) => {
        led.classList.remove('active', 'yellow', 'green');
        if (i < activeLeds) {
            if (i < 6) led.classList.add('green'); else if (i < 12) led.classList.add('yellow'); else led.classList.add('active');
        }
    });
    const valEl = document.getElementById('comp-gr-val');
    if (valEl) valEl.textContent = '-' + grDb.toFixed(1) + ' dB';
    
    if (window.grBuffer) {
        window.grBuffer.push(grDb);
        if (window.grBuffer.length > 200) window.grBuffer.shift();
    }
}

function pushWaveformLevel(level) {
    if (window.waveformBuffer) {
        window.waveformBuffer.push(level);
        if (window.waveformBuffer.length > 200) window.waveformBuffer.shift();
    }
}

function resetComp() {
    compSettings = { threshold: -20, ratio: 4, attack: 10, release: 100, makeup: 0, knee: 6 };
    if (compKnobs['threshold']) compKnobs['threshold'].setValue(-20);
    if (compKnobs['ratio']) compKnobs['ratio'].setValue(4);
    if (compKnobs['attack']) compKnobs['attack'].setValue(10);
    if (compKnobs['release']) compKnobs['release'].setValue(100);
    if (compKnobs['makeup']) compKnobs['makeup'].setValue(0);
    if (compKnobs['knee']) compKnobs['knee'].setValue(6);
    
    drawTransferCurve();
    window.location = 'sphere://comp/threshold/-20';
    setTimeout(() => window.location = 'sphere://comp/ratio/4', 50);
    setTimeout(() => window.location = 'sphere://comp/attack/10', 100);
    setTimeout(() => window.location = 'sphere://comp/release/100', 150);
    setTimeout(() => window.location = 'sphere://comp/makeup/0', 200);
    setTimeout(() => window.location = 'sphere://comp/knee/6', 250);
}

// Canvas init happens via openComp -> initCompressorUI -> initCompCanvases
    )JS";

inline const juce::String js = jsPart1 + jsPart2 + jsPart3 + jsPart4;

inline const juce::String htmlPart1 =
    R"HTML(<!DOCTYPE html><html><head><meta charset="UTF-8"><style>)HTML";
inline const juce::String htmlPart2 = R"HTML(</style></head><body>
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
          <div class="fx-card" onclick="openComp()"><div class="fx-card-header"><span class="fx-card-title">Comp</span><div class="fx-toggle on" id="comp-master-toggle" onclick="event.stopPropagation(); toggleCompMaster()"></div></div><div class="fx-card-preview"><div style="color: rgba(0,229,255,0.7); font-size: 11px;">COMPRESSOR</div></div></div>
        </div>
      </div>
      <div id="eq-panel" class="eq-panel">
        <div class="eq-header"><span class="eq-title">Sphere EQ</span><button class="eq-close-btn" onclick="closeEQ()">&minus;</button></div>
        <div class="eq-display-container">
          <div class="eq-analyzer"><canvas id="eq-analyzer-canvas" class="eq-analyzer-canvas"></canvas><div class="eq-grid-overlay"></div><div class="eq-freq-labels"><span>20</span><span>50</span><span>100</span><span>200</span><span>500</span><span>1k</span><span>2k</span><span>5k</span><span>10k</span><span>20k</span></div><div class="eq-db-labels"><span>+12</span><span>+6</span><span>0</span><span>-6</span><span>-12</span></div></div>
          <div class="eq-bands-container" id="eq-bands-container"></div>
        </div>
      </div>
      <div id="comp-panel" class="comp-panel">
        <div class="comp-header">
          <span class="comp-title">Sphere Compressor</span>
          <div class="comp-header-btns">
            <button class="comp-reset-btn" onclick="resetComp()">Reset</button>
            <button class="comp-close-btn" onclick="closeComp()">&minus;</button>
          </div>
        </div>
        <div class="comp-main">
          <div class="comp-visualizer">
            <canvas id="comp-waveform-canvas" class="comp-waveform-canvas"></canvas>
            <canvas id="comp-transfer-canvas" class="comp-transfer-canvas"></canvas>
            <div class="comp-grid-overlay"></div>
            <div class="comp-input-label">Input (dB)</div>
            <div class="comp-output-label">Output (dB)</div>
          </div>
          <div class="comp-gr-meter">
            <div class="comp-gr-label">GR</div>
            <div class="comp-gr-leds">
              <div class="comp-gr-led"></div><div class="comp-gr-led"></div><div class="comp-gr-led"></div><div class="comp-gr-led"></div>
              <div class="comp-gr-led"></div><div class="comp-gr-led"></div><div class="comp-gr-led"></div><div class="comp-gr-led"></div>
              <div class="comp-gr-led"></div><div class="comp-gr-led"></div><div class="comp-gr-led"></div><div class="comp-gr-led"></div>
              <div class="comp-gr-led"></div><div class="comp-gr-led"></div><div class="comp-gr-led"></div><div class="comp-gr-led"></div>
              <div class="comp-gr-led"></div><div class="comp-gr-led"></div><div class="comp-gr-led"></div><div class="comp-gr-led"></div>
            </div>
            <div class="comp-gr-value" id="comp-gr-val">0.0 dB</div>
          </div>
        </div>
        <div class="comp-control-strip" id="comp-controls"></div>
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
      </body><script>)HTML";

inline const juce::String htmlPart3 = R"HTML(</script></html>)HTML";

inline const juce::String css3 = R"CSS(
/* Pro Control Strip Layout */
.comp-pro-controls { display: flex; align-items: flex-end; justify-content: space-between; padding: 15px 30px; background: linear-gradient(to bottom, #1a1a20, #0f0f12); border-top: 1px solid #333; height: 140px; box-shadow: inset 0 10px 20px rgba(0,0,0,0.5); }
.comp-section { display: flex; gap: 15px; align-items: flex-end; padding: 0 10px; position: relative; }
.comp-section:not(:last-child)::after { content: ''; position: absolute; right: -7px; top: 20px; bottom: 20px; width: 1px; background: linear-gradient(to bottom, transparent, #444, transparent); }
.comp-section-thresh { padding-right: 20px; }
.comp-section-main { flex-grow: 1; justify-content: center; }
.comp-delta-wrapper { display: flex; flex-direction: column; align-items: center; gap: 5px; margin-bottom: 5px; }
.comp-delta-btn { width: 32px; height: 32px; border-radius: 50%; border: 1px solid #444; display: flex; align-items: center; justify-content: center; cursor: pointer; color: #666; background: #222; transition: all 0.2s ease; box-shadow: 0 2px 5px rgba(0,0,0,0.5); }
.comp-delta-btn:hover { border-color: #777; color: #aaa; }
.comp-delta-btn.active { border-color: #00e5ff; color: #00e5ff; background: rgba(0, 229, 255, 0.1); box-shadow: 0 0 10px rgba(0, 229, 255, 0.3); }
.knob-value { margin-top: 8px; text-shadow: 0 1px 2px rgba(0,0,0,0.8); }
.knob-label { margin-top: 2px; }

/* Visualizer Styles */
.comp-visualizer { position: relative; width: 100%; height: 260px; background: #0f0f12; border-bottom: 1px solid #333; overflow: hidden; }
.comp-waveform-canvas, .comp-transfer-canvas { position: absolute; top: 0; left: 0; width: 100%; height: 100%; }
.comp-transfer-canvas { z-index: 1; }
.comp-waveform-canvas { z-index: 2; pointer-events: none; }
.comp-grid-overlay { display: none; }
.comp-input-label { position: absolute; bottom: 5px; left: 50%; transform: translateX(-50%); color: rgba(0,229,255,0.4); font-size: 10px; font-family: monospace; pointer-events: none; z-index: 3; text-transform: uppercase; letter-spacing: 1px; }
.comp-output-label { position: absolute; left: 5px; top: 50%; transform: translateY(-50%) rotate(-90deg); color: rgba(0,229,255,0.4); font-size: 10px; font-family: monospace; pointer-events: none; z-index: 3; text-transform: uppercase; letter-spacing: 1px; }

/* Meter Styles */
.comp-gr-meter { display: flex; flex-direction: column; align-items: center; width: 40px; border-left: 1px solid #333; background: #15151a; padding: 10px 0; }
.comp-gr-label { color: #555; font-size: 10px; margin-bottom: 10px; letter-spacing: 1px; font-weight: bold; }
.comp-gr-leds { display: flex; flex-direction: column-reverse; gap: 2px; width: 12px; height: 180px; background: #0a0a0c; padding: 2px; border-radius: 2px; box-shadow: inset 0 0 5px rgba(0,0,0,0.8); }
.comp-gr-led { width: 100%; height: 6px; background: #222; border-radius: 1px; transition: background 0.05s; }
.comp-gr-led.active { background: #ff3d00; box-shadow: 0 0 5px #ff3d00; }
.comp-gr-led.yellow { background: #ffea00; box-shadow: 0 0 5px #ffea00; }
.comp-gr-led.green { background: #76ff03; box-shadow: 0 0 5px #76ff03; }
.comp-gr-value { margin-top: 10px; color: #ff3d00; font-family: monospace; font-size: 10px; font-weight: bold; }
    )CSS";
inline const juce::String html =
    htmlPart1 + css + css2 + css3 + htmlPart2 + js + htmlPart3;
} // namespace SphereSynthResources
