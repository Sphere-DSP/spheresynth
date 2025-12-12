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
        width: 6px; height: 300px; background: rgba(255, 255, 255, 0.1); border-radius: 3px; z-index: 2000;
        border: 1px solid rgba(255, 255, 255, 0.15);
      }
      .volume-meter-bar { width: 100%; height: 0%; background: linear-gradient(to top, #00e5ff, #00ff88);
        position: absolute; bottom: 0; box-shadow: 0 0 15px #00e5ff; border-radius: 2px;
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
      
      /* ===== PRO-Q3 STYLE EQ PANEL ===== */
      .eq-panel { position: fixed; top: 60px; left: 0; right: 0; bottom: 0;
        background: linear-gradient(180deg, #0a0a12 0%, #050508 100%);
        z-index: 1500; display: none; overflow: hidden;
      }
      .eq-panel.active { display: block; }
      
      /* Full-screen analyzer */
      .eq-spectrum { position: absolute; top: 0; left: 0; right: 0; bottom: 0; }
      .eq-spectrum-canvas { width: 100%; height: 100%; }
      
      /* Grid overlay */
      .eq-grid { position: absolute; top: 0; left: 40px; right: 0; bottom: 30px; pointer-events: none; }
      
      /* Frequency labels - ultra thin */
      .eq-freq-axis { position: absolute; bottom: 4px; left: 36px; right: 8px;
        display: flex; justify-content: space-between; pointer-events: none;
      }
      .eq-freq-label { font-size: 8px; color: rgba(255, 255, 255, 0.18); font-weight: 200; letter-spacing: 0.2px; }
      
      /* dB labels - ultra thin */
      .eq-db-axis { position: absolute; top: 40px; bottom: 20px; left: 4px; width: 26px;
        display: flex; flex-direction: column; justify-content: space-between; align-items: flex-end;
        pointer-events: none;
      }
      .eq-db-label { font-size: 7px; color: rgba(255, 255, 255, 0.15); font-weight: 200; }
      
      /* Toolbar hidden */
      .eq-toolbar { display: none; }
      .eq-tool-btn { display: none; }
      .eq-tool-divider { display: none; }
      
      /* Close button - minimize style */
      .eq-close { position: absolute; top: 8px; right: 10px;
        width: 26px; height: 26px; border-radius: 6px;
        background: rgba(255, 255, 255, 0.02); border: 1px solid rgba(255, 255, 255, 0.05);
        color: rgba(255, 255, 255, 0.3); font-size: 16px; font-weight: 200; cursor: pointer;
        display: flex; align-items: center; justify-content: center; z-index: 100;
      }
      .eq-close:hover { background: rgba(255, 255, 255, 0.06); border-color: rgba(255, 255, 255, 0.1); color: rgba(255, 255, 255, 0.6); }
      
      /* Band nodes on spectrum - ultra thin style */
      .eq-node { position: absolute; width: 18px; height: 18px; border-radius: 50%;
        background: rgba(10, 10, 14, 0.9); border: 1px solid rgba(255, 255, 255, 0.3);
        transform: translate(-50%, -50%); cursor: grab; z-index: 10;
        display: flex; align-items: center; justify-content: center;
        color: rgba(255, 255, 255, 0.7); font-size: 9px; font-weight: 500;
        box-shadow: 0 2px 5px rgba(0, 0, 0, 0.5);
        transition: border-color 0.2s, color 0.2s, transform 0.1s, box-shadow 0.2s;
      }
      .eq-node:hover { transform: translate(-50%, -50%) scale(1.12); 
        box-shadow: 0 0 10px rgba(0, 229, 255, 0.35); border-color: rgba(0, 229, 255, 0.8);
      }
      .eq-node.selected { background: rgba(0, 229, 255, 0.25); transform: translate(-50%, -50%) scale(1.15);
        box-shadow: 0 0 14px rgba(0, 229, 255, 0.45); border-width: 1.5px;
      }
      .eq-node.dynamic { background: rgba(255, 200, 0, 0.12); border-color: rgba(255, 200, 0, 0.5);
        box-shadow: 0 0 6px rgba(255, 200, 0, 0.2);
      }
      .eq-node.bypassed { opacity: 0.2; border-style: dashed; }
      .eq-node-ring { position: absolute; width: 26px; height: 26px; border-radius: 50%;
        border: 1px solid rgba(0, 229, 255, 0.12); pointer-events: none;
      }
      
      /* Floating band control panel - Modern Apple-style design - COMPACT & TRANSPARENT */
      .eq-band-panel { 
        position: absolute !important; bottom: 12px !important; left: 50% !important; right: auto !important;
        top: auto !important; transform: translateX(-50%) !important;
        background: rgba(25, 25, 32, 0.5); backdrop-filter: blur(20px) saturate(180%);
        border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 12px;
        padding: 6px 10px; z-index: 200;
        display: none; flex-direction: column; gap: 6px;
        box-shadow: 0 4px 16px rgba(0, 0, 0, 0.3), inset 0 1px 0 rgba(255, 255, 255, 0.05);
      }
      .eq-band-panel.active { display: flex !important; }
      
      /* Top row: Power, Dropdown, Delete */
      .eq-bp-top-row { display: flex; align-items: center; gap: 6px; justify-content: center; }
      
      /* Modern Power Button - ROUND with shimmer */
      .eq-power-btn { 
        width: 22px !important; height: 22px !important; min-width: 22px; min-height: 22px;
        border-radius: 50% !important; padding: 0 !important;
        background: linear-gradient(135deg, rgb(60, 60, 70), rgb(40, 40, 50));
        border: 1px solid rgba(255, 255, 255, 0.2);
        color: rgba(255, 255, 255, 0.5); cursor: pointer;
        display: flex; align-items: center; justify-content: center;
        transition: all 0.25s ease;
        box-shadow: 0 2px 6px rgba(0, 0, 0, 0.4), inset 0 1px 0 rgba(255, 255, 255, 0.1);
        position: relative; overflow: hidden; flex-shrink: 0;
      }
      .eq-power-btn::before {
        content: ''; position: absolute; inset: 0; border-radius: 50%;
        background: radial-gradient(circle at 30% 30%, rgba(255, 255, 255, 0.15), transparent 60%);
        pointer-events: none;
      }
      .eq-power-btn svg { width: 10px; height: 10px; stroke: currentColor; stroke-width: 2; fill: none; position: relative; z-index: 1; }
      .eq-power-btn:hover { 
        background: linear-gradient(135deg, rgba(70, 70, 80, 0.95), rgba(50, 50, 60, 0.95));
        color: rgba(255, 255, 255, 0.8); 
        box-shadow: 0 2px 8px rgba(0, 0, 0, 0.4), 0 0 12px rgba(255, 255, 255, 0.1);
      }
      .eq-power-btn.on { 
        background: linear-gradient(135deg, rgba(0, 180, 220, 0.4), rgba(0, 120, 160, 0.5));
        border-color: rgba(0, 229, 255, 0.5);
        color: rgba(0, 229, 255, 1);
        box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3), 0 0 16px rgba(0, 229, 255, 0.4), inset 0 0 8px rgba(0, 229, 255, 0.2);
      }
      
      /* Modern Delete Button - ROUND with shimmer */
      .eq-delete-btn { 
        width: 22px !important; height: 22px !important; min-width: 22px; min-height: 22px;
        border-radius: 50% !important; padding: 0 !important;
        background: linear-gradient(135deg, rgb(60, 60, 70), rgb(40, 40, 50));
        border: 1px solid rgba(255, 255, 255, 0.2);
        color: rgba(255, 120, 120, 0.7); cursor: pointer;
        display: flex; align-items: center; justify-content: center;
        transition: all 0.25s ease;
        box-shadow: 0 2px 6px rgba(0, 0, 0, 0.4), inset 0 1px 0 rgba(255, 255, 255, 0.1);
        position: relative; overflow: hidden; flex-shrink: 0;
      }
      .eq-delete-btn::before {
        content: ''; position: absolute; inset: 0; border-radius: 50%;
        background: radial-gradient(circle at 30% 30%, rgba(255, 255, 255, 0.15), transparent 60%);
        pointer-events: none;
      }
      .eq-delete-btn svg { width: 8px; height: 8px; stroke: currentColor; stroke-width: 2.5; fill: none; position: relative; z-index: 1; }
      .eq-delete-btn:hover { 
        background: linear-gradient(135deg, rgba(120, 50, 50, 0.6), rgba(80, 30, 30, 0.7));
        border-color: rgba(255, 100, 100, 0.5);
        color: rgba(255, 120, 120, 1);
        box-shadow: 0 2px 8px rgba(0, 0, 0, 0.4), 0 0 12px rgba(255, 80, 80, 0.3);
      }
      
      /* Curve Type Dropdown - Apple-style - COMPACT */
      .eq-type-dropdown { position: relative; }
      .eq-dropdown-trigger { 
        display: flex; align-items: center; gap: 6px; padding: 4px 8px;
        background: rgba(255, 255, 255, 0.04); border: 1px solid rgba(255, 255, 255, 0.08);
        border-radius: 6px; cursor: pointer; transition: all 0.2s ease;
        color: rgba(255, 255, 255, 0.8); font-size: 10px; font-weight: 500;
      }
      .eq-dropdown-trigger:hover { background: rgba(255, 255, 255, 0.08); }
      .eq-dropdown-icon { width: 16px; height: 12px; stroke: rgba(0, 229, 255, 0.9); stroke-width: 2; fill: none; }
      .eq-dropdown-label { min-width: 50px; text-align: left; }
      .eq-dropdown-arrow { width: 8px; height: 8px; stroke: rgba(255, 255, 255, 0.4); stroke-width: 2; fill: none; transition: transform 0.2s; }
      .eq-type-dropdown.open .eq-dropdown-arrow { transform: rotate(180deg); }
      
      .eq-dropdown-menu { 
        position: absolute; bottom: calc(100% + 6px); left: 50%; transform: translateX(-50%); top: auto;
        background: rgba(40, 40, 45, 0.98); backdrop-filter: blur(20px);
        border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 10px;
        padding: 4px; min-width: 140px; z-index: 300;
        box-shadow: 0 -8px 24px rgba(0, 0, 0, 0.4);
        display: none; flex-direction: column;
      }
      .eq-type-dropdown.open .eq-dropdown-menu { display: flex; }
      
      .eq-dropdown-item { 
        display: flex; align-items: center; gap: 10px; padding: 8px 12px;
        background: transparent; border: none; border-radius: 6px;
        color: rgba(255, 255, 255, 0.75); font-size: 11px; font-weight: 400;
        cursor: pointer; transition: all 0.15s ease; text-align: left;
      }
      .eq-dropdown-item svg { width: 20px; height: 14px; stroke: currentColor; stroke-width: 2; fill: none; flex-shrink: 0; }
      .eq-dropdown-item:hover { background: rgba(255, 255, 255, 0.08); color: rgba(255, 255, 255, 1); }
      .eq-dropdown-item.active { 
        background: rgba(0, 229, 255, 0.15); color: rgba(0, 229, 255, 1);
      }
      .eq-dropdown-item.active svg { stroke: rgba(0, 229, 255, 1); }
      
      /* Knobs row */
      .eq-knobs-row { display: flex; gap: 16px; align-items: flex-start; justify-content: center; }
      
      /* Stereo Mode Dropdown - Compact pill style */
      .eq-stereo-dropdown { position: relative; margin-left: 4px; }
      .eq-stereo-trigger { 
        display: flex; align-items: center; gap: 4px; padding: 3px 8px;
        background: rgb(50, 50, 58); border: 1px solid rgba(255, 255, 255, 0.15);
        border-radius: 10px; cursor: pointer; transition: all 0.2s ease;
        color: rgba(255, 255, 255, 0.7); font-size: 9px; font-weight: 500;
        text-transform: uppercase; letter-spacing: 0.3px;
      }
      .eq-stereo-trigger:hover { background: rgb(60, 60, 68); border-color: rgba(255, 255, 255, 0.25); }
      .eq-stereo-trigger.mid { border-color: rgba(100, 200, 150, 0.5); color: rgba(100, 200, 150, 0.9); }
      .eq-stereo-trigger.side { border-color: rgba(200, 150, 100, 0.5); color: rgba(200, 150, 100, 0.9); }
      .eq-stereo-trigger.left { border-color: rgba(100, 180, 255, 0.5); color: rgba(100, 180, 255, 0.9); }
      .eq-stereo-trigger.right { border-color: rgba(255, 100, 180, 0.5); color: rgba(255, 100, 180, 0.9); }
      .eq-stereo-arrow { width: 6px; height: 6px; stroke: currentColor; stroke-width: 2; fill: none; transition: transform 0.2s; }
      .eq-stereo-dropdown.open .eq-stereo-arrow { transform: rotate(180deg); }
      
      .eq-stereo-menu { 
        position: absolute; bottom: calc(100% + 4px); left: 0; top: auto;
        background: rgb(45, 45, 52); backdrop-filter: blur(20px);
        border: 1px solid rgba(255, 255, 255, 0.12); border-radius: 8px;
        padding: 3px; min-width: 80px; z-index: 310;
        box-shadow: 0 -6px 20px rgba(0, 0, 0, 0.5);
        display: none; flex-direction: column;
      }
      .eq-stereo-dropdown.open .eq-stereo-menu { display: flex; }
      
      .eq-stereo-item { 
        display: flex; align-items: center; gap: 6px; padding: 5px 10px;
        background: transparent; border: none; border-radius: 5px;
        color: rgba(255, 255, 255, 0.7); font-size: 10px; font-weight: 400;
        cursor: pointer; transition: all 0.15s ease; text-align: left;
      }
      .eq-stereo-item:hover { background: rgba(255, 255, 255, 0.08); color: rgba(255, 255, 255, 1); }
      .eq-stereo-item.active { background: rgba(0, 229, 255, 0.15); color: rgba(0, 229, 255, 1); }
      
      /* Slope Dropdown - Compact pill style */
      .eq-slope-dropdown { position: relative; margin-left: 4px; }
      .eq-slope-trigger { 
        display: flex; align-items: center; gap: 3px; padding: 3px 7px;
        background: rgb(50, 50, 58); border: 1px solid rgba(255, 255, 255, 0.15);
        border-radius: 10px; cursor: pointer; transition: all 0.2s ease;
        color: rgba(255, 255, 255, 0.6); font-size: 9px; font-weight: 500;
      }
      .eq-slope-trigger:hover { background: rgb(60, 60, 68); border-color: rgba(255, 255, 255, 0.25); }
      .eq-slope-arrow { width: 5px; height: 5px; stroke: currentColor; stroke-width: 2; fill: none; transition: transform 0.2s; }
      .eq-slope-dropdown.open .eq-slope-arrow { transform: rotate(180deg); }
      
      .eq-slope-menu { 
        position: absolute; bottom: calc(100% + 4px); left: 0; top: auto;
        background: rgb(45, 45, 52); backdrop-filter: blur(20px);
        border: 1px solid rgba(255, 255, 255, 0.12); border-radius: 8px;
        padding: 3px; min-width: 60px; z-index: 310;
        box-shadow: 0 -6px 20px rgba(0, 0, 0, 0.5);
        display: none; flex-direction: column;
      }
      .eq-slope-dropdown.open .eq-slope-menu { display: flex; }
      
      .eq-slope-item { 
        padding: 5px 10px; background: transparent; border: none; border-radius: 5px;
        color: rgba(255, 255, 255, 0.7); font-size: 10px; font-weight: 400;
        cursor: pointer; transition: all 0.15s ease; text-align: left;
      }
      .eq-slope-item:hover { background: rgba(255, 255, 255, 0.08); color: rgba(255, 255, 255, 1); }
      .eq-slope-item.active { background: rgba(0, 229, 255, 0.15); color: rgba(0, 229, 255, 1); }

      
      /* Band panel header - legacy (hidden) */
      .eq-bp-header { display: none; }
      .eq-bp-title { display: none; }
      .eq-bp-close { display: none; }
      
      /* Delete band button - legacy (replaced by eq-delete-btn) */
      .eq-bp-delete { display: none; }
      
      /* Pill buttons - legacy (replaced by dropdown) */
      .eq-pill-group { display: none; }
      .eq-pill { display: none; }
      
      /* Rotary Knob Controls with Shimmer - COMPACT SIZE */
      .eq-knob-group { display: flex; flex-direction: column; align-items: center; gap: 4px; }
      .eq-knob-label { font-size: 9px; color: rgba(255, 255, 255, 0.4); text-transform: uppercase; letter-spacing: 0.5px; font-weight: 400; }
      .eq-knob-value { font-size: 10px; color: rgba(255, 255, 255, 0.75); font-weight: 400; }
      .eq-knob-container { position: relative; width: 56px; height: 56px; }
      .eq-knob { width: 56px; height: 56px; border-radius: 50%;
        background: radial-gradient(circle at 30% 30%, rgb(45, 45, 55), rgb(20, 20, 28));
        border: 2px solid rgba(255, 255, 255, 0.18);
        position: relative; cursor: grab; user-select: none;
        box-shadow: 0 3px 10px rgba(0, 0, 0, 0.6), inset 0 1px 0 rgba(255, 255, 255, 0.08);
      }
      .eq-knob::before { content: ''; position: absolute; top: 50%; left: 50%;
        width: 3px; height: 16px; background: rgba(255, 255, 255, 0.6);
        border-radius: 2px; transform-origin: center bottom;
        transform: translate(-50%, -100%);
      }
      .eq-knob-shimmer { position: absolute; top: -2px; left: -2px; width: 60px; height: 60px;
        border-radius: 50%; pointer-events: none;
        /* Mask defines the arc shape - set by JS */
        -webkit-mask-repeat: no-repeat;
        mask-repeat: no-repeat;
        /* Background is the base color - set by JS */
        filter: blur(0.5px);
        opacity: 1.0;
      }
      /* Moving light effect inside the masked area */
      .eq-knob-shimmer::after {
        content: ''; position: absolute; top: 0; left: 0; width: 100%; height: 100%;
        border-radius: 50%;
        /* Wider, softer gradient for a premium feel */
        background: conic-gradient(from 0deg, transparent 0deg, rgba(255, 255, 255, 0.0) 20deg, rgba(255, 255, 255, 0.8) 60deg, rgba(255, 255, 255, 0.0) 100deg, transparent 360deg);
        /* Slower, smoother animation */
        animation: shimmer-rotate 4s linear infinite;
        mix-blend-mode: overlay;
        filter: blur(3px); /* Add blur for softness */
      }
      @keyframes shimmer-rotate {
        0% { transform: rotate(0deg); }
        100% { transform: rotate(360deg); }
      }
      .eq-knob:hover { border-color: rgba(0, 229, 255, 0.3); }
      .eq-knob:active { cursor: grabbing; }
      
      /* Dynamic Gain Knob - Yellow/Gold styling - ALWAYS INTERACTIVE */
      .eq-knob-group.dynamic-knob { opacity: 0.35; transition: opacity 0.3s; }
      .eq-knob-group.dynamic-knob.active { opacity: 1; }
      .eq-knob-label.dynamic { color: rgba(255, 200, 0, 0.5); }
      .eq-knob-value.dynamic { color: rgba(255, 200, 0, 0.7); }
      .eq-knob.dynamic { 
        background: radial-gradient(circle at 30% 30%, rgba(50, 45, 30, 0.9), rgba(25, 20, 10, 0.95));
        border: 2px solid rgba(255, 200, 0, 0.25);
        cursor: grab;
      }
      .eq-knob.dynamic:hover { border-color: rgba(255, 200, 0, 0.5); }
      .eq-knob.dynamic::before { background: rgba(255, 200, 0, 0.7); }
      .eq-knob-shimmer.dynamic {
        background: conic-gradient(from 135deg, transparent 0deg, rgba(255, 200, 0, 0.45) var(--knob-angle, 0deg), transparent var(--knob-angle, 0deg));
      }
      @keyframes dyn-knob-shimmer {
        0%, 100% { opacity: 0.5; filter: blur(4px); }
        50% { opacity: 0.85; filter: blur(5px); }
      }
      .eq-knob-group.dynamic-knob.active .eq-knob-label.dynamic { color: rgba(255, 200, 0, 0.8); }
      .eq-knob-group.dynamic-knob.active .eq-knob.dynamic { border-color: rgba(255, 200, 0, 0.5); box-shadow: 0 0 18px rgba(255, 200, 0, 0.3); }
      .eq-knob-group.dynamic-knob.active .eq-knob-shimmer.dynamic { animation-name: dyn-knob-shimmer; }
      
      /* Legacy slider (for dynamic section) */
      .eq-slider-group { display: flex; flex-direction: column; gap: 2px; min-width: 70px; }
      .eq-slider-header { display: flex; justify-content: space-between; align-items: baseline; }
      .eq-slider-label { font-size: 7px; color: rgba(255, 255, 255, 0.25); text-transform: uppercase; letter-spacing: 0.4px; font-weight: 300; }
      .eq-slider-value { font-size: 9px; color: rgba(255, 255, 255, 0.6); font-weight: 300; }
      .eq-slider-track { position: relative; height: 2px; background: rgba(255, 255, 255, 0.06);
        border-radius: 1px; cursor: pointer;
      }
      .eq-slider-fill { position: absolute; top: 0; left: 0; height: 100%;
        background: rgba(0, 229, 255, 0.5); border-radius: 1px;
      }
      .eq-slider-thumb { position: absolute; top: 50%; width: 10px; height: 10px;
        background: rgba(255, 255, 255, 0.8); border-radius: 50%; transform: translate(-50%, -50%);
        box-shadow: 0 1px 3px rgba(0, 0, 0, 0.25); cursor: grab;
      }
      
      /* (Old power button CSS removed - now in modern styles above) */
      
      /* Legacy toggle switch - hidden */
      .eq-toggle { display: none; }
      .eq-toggle-label { display: none; }
      .eq-toggle-switch { display: none; }
      
      /* Dynamic section in band panel */
      .eq-bp-dynamic { padding-left: 16px; border-left: 1px solid rgba(255, 255, 255, 0.04); display: flex; align-items: center; gap: 14px; }
      .eq-bp-dynamic-header { display: flex; align-items: center; gap: 8px; }
      .eq-bp-dynamic-title { font-size: 8px; color: rgba(255, 200, 0, 0.5); font-weight: 300; text-transform: uppercase; letter-spacing: 0.4px; }
      
      /* Add band button - hidden */
      .eq-add-band { display: none; }
      
      /* Bottom Bar */
      .eq-bottom-bar {
        position: absolute; bottom: 0; left: 0; width: 100%; height: 40px;
        background: rgba(10, 10, 14, 0.8); backdrop-filter: blur(10px);
        border-top: 1px solid rgba(255, 255, 255, 0.05);
        display: flex; align-items: center; justify-content: space-between;
        padding: 0 10px; z-index: 90;
      }

      /* Output/Phase controls - positioned within bottom bar */
      .eq-controls-left { display: flex; gap: 4px; z-index: 100; }
      .eq-controls-right { display: flex; gap: 4px; z-index: 100; }
      
      .eq-output-btn { padding: 5px 10px; border-radius: 4px;
        background: rgba(12, 12, 18, 0.4); backdrop-filter: blur(20px);
        border: 1px solid rgba(255, 255, 255, 0.04);
        color: rgba(255, 255, 255, 0.35); font-size: 8px; font-weight: 300;
        cursor: pointer; text-transform: uppercase; letter-spacing: 0.4px;
      }
      
      /* Dropdown Styles for Scale/Oversampling */
      .eq-dropdown { position: relative; }
      .eq-dropdown-menu { 
        position: absolute; bottom: calc(100% + 4px); left: 50%; top: auto;
        transform: translateX(-50%);
        background: rgb(45, 45, 52); backdrop-filter: blur(20px);
        border: 1px solid rgba(255, 255, 255, 0.12); border-radius: 8px;
        padding: 3px; min-width: 70px; z-index: 310;
        box-shadow: 0 -6px 20px rgba(0, 0, 0, 0.5);
        display: none; flex-direction: column;
      }


      .eq-dropdown.open .eq-dropdown-menu { display: flex; }
      
      .eq-dropdown-item { 
        padding: 5px 10px; background: transparent; border: none; border-radius: 5px;
        color: rgba(255, 255, 255, 0.7); font-size: 9px; font-weight: 400;
        cursor: pointer; transition: all 0.15s ease; text-align: left;
        white-space: nowrap;
      }
      .eq-dropdown-item:hover { background: rgba(255, 255, 255, 0.08); color: rgba(255, 255, 255, 1); }
      .eq-dropdown-item.active { background: rgba(0, 229, 255, 0.15); color: rgba(0, 229, 255, 1); }
      .eq-output-btn:hover { background: rgba(18, 18, 25, 0.6); border-color: rgba(255, 255, 255, 0.08); color: rgba(255, 255, 255, 0.55); }
      .eq-output-btn.active { background: rgba(0, 229, 255, 0.08); border-color: rgba(0, 229, 255, 0.2); color: rgba(0, 229, 255, 0.75); }
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
            if (e.repeat) { e.preventDefault(); return; }
            if (e.code === 'KeyZ') { e.preventDefault(); octaveOffset = Math.max(24, octaveOffset - 12); return; }
            if (e.code === 'KeyX') { e.preventDefault(); octaveOffset = Math.min(84, octaveOffset + 12); return; }
            const noteOffset = baseKeyboardMap[e.code];
            if (noteOffset !== undefined && !pressedKeys.has(e.code)) {
                e.preventDefault();
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
            // Only update main volume bar if it exists (it's handled by JUCE usually but we might have a JS visual)
            // The horizontal meter is removed.
            const bar = document.getElementById('volume-bar');
            if (bar) {
                const percentage = Math.min(100, Math.max(0, level * 100));
                bar.style.height = percentage + '%';
                bar.style.opacity = 0.5 + (level * 0.5);
            }
        }

        function updateSpectrum(input, output) {
            analyzerData.input = input;
            analyzerData.output = output;
            drawEQSpectrum();
        }

        function toggleScaleDropdown() {
            const dropdown = document.getElementById('eq-scale-dropdown');
            if (dropdown) dropdown.classList.toggle('open');
            closeOversamplingDropdown();
        }

        function setScale(val) {
            analyzerScale = val;
            const label = document.getElementById('eq-scale-label');
            if (label) label.textContent = val + 'dB';
            
            // Update active state
            document.querySelectorAll('#eq-scale-menu .eq-dropdown-item').forEach(btn => {
                btn.classList.toggle('active', parseInt(btn.textContent) === val);
            });

            const dropdown = document.getElementById('eq-scale-dropdown');
            if (dropdown) dropdown.classList.remove('open');
            
            drawEQSpectrum();
        }

        function toggleOversamplingDropdown() {
            const dropdown = document.getElementById('eq-oversampling-dropdown');
            if (dropdown) dropdown.classList.toggle('open');
            const scaleDropdown = document.getElementById('eq-scale-dropdown');
            if (scaleDropdown) scaleDropdown.classList.remove('open');
        }

        function closeOversamplingDropdown() {
            const dropdown = document.getElementById('eq-oversampling-dropdown');
            if (dropdown) dropdown.classList.remove('open');
        }

        function setOversampling(val) {
            // Update label
            const label = document.getElementById('eq-oversampling-label');
            if (label) label.textContent = val === 0 ? 'OFF' : val + 'x';
            
            // Update active state
            document.querySelectorAll('#eq-oversampling-menu .eq-dropdown-item').forEach(btn => {
                const btnVal = btn.textContent === 'OFF' ? 0 : parseInt(btn.textContent);
                btn.classList.toggle('active', btnVal === val);
            });

            const dropdown = document.getElementById('eq-oversampling-dropdown');
            if (dropdown) dropdown.classList.remove('open');

            // Send to C++
            window.location = 'sphere://eq/oversampling/' + val;
        }
        // updateVolumeBar removed - merged into updateMeter
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
        
        // ===== PRO-Q3 STYLE EQ ENGINE =====
        let eqEnabled = true;
        let selectedBandIndex = -1;
        let currentToolMode = 'bell';
        let isDraggingNode = false;
        let eqCanvas, eqCtx;
        const dynamicGainReductions = new Array(10).fill(0);
        
        // Band color palette (10 unique colors)
        const bandColors = [
            { r: 0, g: 229, b: 255, hex: '#00e5ff' },     // 1. Cyan
            { r: 180, g: 100, b: 255, hex: '#b464ff' },   // 2. Purple
            { r: 255, g: 200, b: 0, hex: '#ffc800' },     // 3. Yellow/Gold
            { r: 0, g: 255, b: 180, hex: '#00ffb4' },     // 4. Teal/Mint
            { r: 255, g: 100, b: 150, hex: '#ff6496' },   // 5. Pink
            { r: 100, g: 200, b: 255, hex: '#64c8ff' },   // 6. Light Blue
            { r: 255, g: 150, b: 50, hex: '#ff9632' },    // 7. Orange
            { r: 150, g: 255, b: 100, hex: '#96ff64' },   // 8. Lime Green
            { r: 255, g: 100, b: 100, hex: '#ff6464' },   // 9. Coral Red
            { r: 200, g: 150, b: 255, hex: '#c896ff' }    // 10. Lavender
        ];
        
        function getBandColor(index) {
            return bandColors[index % bandColors.length];
        }
        
        let eqBands = [];
        let analyzerData = { input: [], output: [] };
        let analyzerScale = 12; // +/- dB range
        let analyzerEnabled = true; // Analyzer enabled by default
        
        // Initialize bands
        for (let i = 0; i < 8; i++) {
            eqBands.push({ active: false, type: 'bell', freq: 1000, gain: 0, q: 1.0, bypass: false, dyn: { active: false, range: 0, mode: 'off', thresh: -20, ratio: 2, attack: 10, release: 100 }, stereoMode: 'stereo', slope: 12 });
        }
        eqBands[0].active = true; eqBands[0].freq = 100; eqBands[0].gain = 3;
        eqBands[1].active = true; eqBands[1].freq = 1000; eqBands[1].gain = 0;
        
        // Open/Close EQ
        function openEQ() {
            const panel = document.getElementById('eq-panel');
            if (panel) {
                panel.classList.add('active');
                setTimeout(() => { initEQSpectrum(); updateBandNodes(); }, 50);
            }
        }
        function closeEQ() {
            const panel = document.getElementById('eq-panel');
            if (panel) panel.classList.remove('active');
            closeBandPanel();
        }
        
        // Initialize spectrum canvas
        // Dynamic curve dragging state
        let isDraggingDynamicCurve = false;
        let dynamicDragBandIndex = -1;
        let dynamicDragStartY = 0;
        let dynamicDragStartGain = 0;
        
        function initEQSpectrum() {
            eqCanvas = document.getElementById('eq-spectrum-canvas');
            if (!eqCanvas) return;
            eqCtx = eqCanvas.getContext('2d');
            resizeEQCanvas();
            window.addEventListener('resize', resizeEQCanvas);
            
            // Canvas click to add band or select
            eqCanvas.addEventListener('click', onCanvasClick);
            eqCanvas.addEventListener('dblclick', onCanvasDoubleClick);
            eqCanvas.addEventListener('mousedown', onDynamicCurveMouseDown);
            document.addEventListener('mousemove', onDynamicCurveMouseMove);
            document.addEventListener('mouseup', onDynamicCurveMouseUp);
        }
        
        // Find dynamic band near cursor for dragging yellow curve
        function findDynamicBandNearCursor(clickX, clickY, graphW, graphH) {
            for (let i = 0; i < eqBands.length; i++) {
                const band = eqBands[i];
                if (!band.active || !band.dyn.active) continue;
                
                const bandX = freqToX(band.freq, graphW);
                const staticGain = band.gain;
                const dynamicGain = staticGain + dynamicGainReductions[i];
                const dynamicY = dbToY(dynamicGain, graphH);
                
                const dist = Math.sqrt(Math.pow(clickX - bandX, 2) + Math.pow(clickY - dynamicY, 2));
                if (dist < 25) {
                    return i;
                }
            }
            return -1;
        }
        
        function onDynamicCurveMouseDown(e) {
            const rect = eqCanvas.getBoundingClientRect();
            const padding = { left: 40, right: 10, top: 50, bottom: 60 };
            const graphW = rect.width - padding.left - padding.right;
            const graphH = rect.height - padding.top - padding.bottom;
            
            const clickX = e.clientX - rect.left - padding.left;
            const clickY = e.clientY - rect.top - padding.top;
            
            const dynBandIdx = findDynamicBandNearCursor(clickX, clickY, graphW, graphH);
            if (dynBandIdx !== -1) {
                isDraggingDynamicCurve = true;
                dynamicDragBandIndex = dynBandIdx;
                dynamicDragStartY = e.clientY;
                dynamicDragStartGain = dynamicGainReductions[dynBandIdx];
                e.preventDefault();
                e.stopPropagation();
            }
        }
        
        function onDynamicCurveMouseMove(e) {
            if (!isDraggingDynamicCurve || dynamicDragBandIndex === -1) return;
            
            const rect = eqCanvas.getBoundingClientRect();
            const graphH = rect.height - 80; // Approximate
            const deltaY = e.clientY - dynamicDragStartY;
            const deltaDb = (deltaY / graphH) * 36; // 36dB range
            
            let newGain = dynamicDragStartGain - deltaDb;
            newGain = Math.max(-18, Math.min(0, newGain)); // Clamp to -18 to 0 dB
            
            dynamicGainReductions[dynamicDragBandIndex] = newGain;
            drawEQSpectrum();
        }
        
        function onDynamicCurveMouseUp(e) {
            if (isDraggingDynamicCurve && dynamicDragBandIndex !== -1) {
                sendDynUpdate(dynamicDragBandIndex);
            }
            isDraggingDynamicCurve = false;
            dynamicDragBandIndex = -1;
        }
        
        function resizeEQCanvas() {
            if (!eqCanvas) return;
            const dpr = window.devicePixelRatio || 1;
            const rect = eqCanvas.parentElement.getBoundingClientRect();
            eqCanvas.width = rect.width * dpr;
            eqCanvas.height = rect.height * dpr;
            eqCtx.scale(dpr, dpr);
            eqCanvas.style.width = rect.width + 'px';
            eqCanvas.style.height = rect.height + 'px';
            drawEQSpectrum();
            updateBandNodes();
        }
        
        // Draw spectrum
        function drawEQSpectrum() {
            if (!eqCtx || !eqCanvas) return;
            const w = eqCanvas.width / (window.devicePixelRatio || 1);
            const h = eqCanvas.height / (window.devicePixelRatio || 1);
            const padding = { left: 40, right: 10, top: 50, bottom: 60 };
            const graphW = w - padding.left - padding.right;
            const graphH = h - padding.top - padding.bottom;
            
            eqCtx.clearRect(0, 0, w, h);
            
            // Background gradient
            const bgGrad = eqCtx.createLinearGradient(0, 0, 0, h);
            bgGrad.addColorStop(0, '#0d0d15');
            bgGrad.addColorStop(1, '#050508');
            eqCtx.fillStyle = bgGrad;
            eqCtx.fillRect(0, 0, w, h);
            
            // Draw grid - ultra thin lines
            eqCtx.strokeStyle = 'rgba(255, 255, 255, 0.025)';
            eqCtx.lineWidth = 0.35;
            
            // Frequency lines (log scale)
            const freqs = [20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000];
            freqs.forEach(f => {
                const x = padding.left + freqToX(f, graphW);
                eqCtx.beginPath();
                eqCtx.moveTo(x, padding.top);
                eqCtx.lineTo(x, h - padding.bottom);
                eqCtx.stroke();
            });
            
            // dB lines
            // dB lines and labels
            let dbs = [];
            if (analyzerScale <= 3) dbs = [-3, -2, -1, 0, 1, 2, 3];
            else if (analyzerScale <= 6) dbs = [-6, -4, -2, 0, 2, 4, 6];
            else if (analyzerScale <= 12) dbs = [-12, -9, -6, -3, 0, 3, 6, 9, 12];
            else if (analyzerScale <= 24) dbs = [-24, -18, -12, -6, 0, 6, 12, 18, 24];
            else dbs = [-36, -24, -12, 0, 12, 24, 36];
            
            eqCtx.font = '10px Inter, sans-serif';
            eqCtx.textAlign = 'right';
            eqCtx.textBaseline = 'middle';

            dbs.forEach(db => {
                const y = padding.top + dbToY(db, graphH);
                eqCtx.strokeStyle = db === 0 ? 'rgba(255, 255, 255, 0.1)' : 'rgba(255, 255, 255, 0.03)';
                eqCtx.lineWidth = db === 0 ? 1 : 0.5;
                eqCtx.beginPath();
                eqCtx.moveTo(padding.left, y);
                eqCtx.lineTo(w - padding.right, y);
                eqCtx.stroke();

                // Draw Label
                eqCtx.fillStyle = db === 0 ? 'rgba(255, 255, 255, 0.8)' : 'rgba(255, 255, 255, 0.4)';
                eqCtx.fillText(db > 0 ? '+' + db : db, padding.left - 5, y);
            });

            // Frequency Labels
            eqCtx.textAlign = 'center';
            eqCtx.textBaseline = 'top';
            freqs.forEach(f => {
                const x = padding.left + freqToX(f, graphW);
                let label = f >= 1000 ? (f/1000) + 'k' : f;
                eqCtx.fillStyle = 'rgba(255, 255, 255, 0.4)';
                eqCtx.fillText(label, x, h - padding.bottom + 5);
            });

            // Draw Analyzer Spectrum (only if enabled)
            if (analyzerEnabled && analyzerData.input.length > 0) {
                // Input Spectrum (Filled)
                eqCtx.fillStyle = 'rgba(255, 255, 255, 0.05)';
                eqCtx.beginPath();
                eqCtx.moveTo(padding.left, h - padding.bottom);
                
                for (let i = 0; i < analyzerData.input.length; i++) {
                    const freq = (i / analyzerData.input.length) * 22050; // Approx Nyquist
                    if (freq < 20) continue;
                    
                    const x = padding.left + freqToX(freq, graphW);
                    const db = analyzerData.input[i];
                    const y = padding.top + spectrumDbToY(db, graphH); // Use spectrum scale
                    
                    eqCtx.lineTo(x, y);
                }
                eqCtx.lineTo(w - padding.right, h - padding.bottom);
                eqCtx.fill();

                // Output Spectrum (Line)
                eqCtx.strokeStyle = 'rgba(255, 255, 255, 0.3)'; // Brighter
                eqCtx.lineWidth = 0.5;
                eqCtx.beginPath();
                
                for (let i = 0; i < analyzerData.output.length; i++) {
                    const freq = (i / analyzerData.output.length) * 22050;
                    if (freq < 20) continue;
                    
                    const x = padding.left + freqToX(freq, graphW);
                    const db = analyzerData.output[i];
                    const y = padding.top + spectrumDbToY(db, graphH); // Use spectrum scale
                    
                    if (i === 0) eqCtx.moveTo(x, y);
                    else eqCtx.lineTo(x, y);
                }
                eqCtx.stroke();
            }
            
            // Draw dynamic curve if any active
            const hasDynamic = eqBands.some(b => b.active && b.dyn && b.dyn.active);
            if (hasDynamic) {
                eqCtx.strokeStyle = 'rgba(255, 200, 0, 0.4)';
                eqCtx.lineWidth = 0.8;
                eqCtx.beginPath();
                for (let px = 0; px <= graphW; px += 2) {
                    const freq = xToFreq(px, graphW);
                    let gain = 0;
                    eqBands.forEach((b, i) => {
                        if (b.active && !b.bypass) {
                            let bandGain = calculateBandResponse(b, freq);
                            if (b.dyn && b.dyn.active && dynamicGainReductions[i] !== 0) {
                                const ratio = freq / b.freq;
                                const shape = Math.exp(-Math.pow(Math.log2(ratio) * b.q, 2) * 2);
                                bandGain += dynamicGainReductions[i] * shape;
                            }
                            gain += bandGain;
                        }
                    });
                    const x = padding.left + px;
                    const y = padding.top + dbToY(gain, graphH);
                    if (px === 0) eqCtx.moveTo(x, y);
                    else eqCtx.lineTo(x, y);
                }
                eqCtx.stroke();
            }
            
            // Draw stereo mode curves (Mid/Side/Left/Right bands get their own colored curves)
            const stereoModeColors = {
                'mid': 'rgba(100, 200, 150, 0.6)',
                'side': 'rgba(200, 150, 100, 0.6)',
                'left': 'rgba(100, 180, 255, 0.6)',
                'right': 'rgba(255, 100, 180, 0.6)'
            };
            
            ['mid', 'side', 'left', 'right'].forEach(mode => {
                const modeBands = eqBands.filter(b => b.active && !b.bypass && b.stereoMode === mode);
                if (modeBands.length === 0) return;
                
                eqCtx.strokeStyle = stereoModeColors[mode];
                eqCtx.lineWidth = 0.8;
                eqCtx.shadowBlur = 4;
                eqCtx.shadowColor = stereoModeColors[mode].replace('0.6', '0.3');
                eqCtx.beginPath();
                
                for (let px = 0; px <= graphW; px += 2) {
                    const freq = xToFreq(px, graphW);
                    let gain = 0;
                    gain = calculateSummedResponse(modeBands, freq);
                    const x = padding.left + px;
                    const y = padding.top + dbToY(gain, graphH);
                    if (px === 0) eqCtx.moveTo(x, y);
                    else eqCtx.lineTo(x, y);
                }
                eqCtx.stroke();
            });
            eqCtx.shadowBlur = 0;
            
            // Draw main EQ curve with shimmer effect
            // Pre-filter main bands for performance
            const mainBands = eqBands.filter(b => b.active && !b.bypass && (!b.stereoMode || b.stereoMode === 'stereo'));

            // First pass - outer glow
            eqCtx.strokeStyle = 'rgba(0, 229, 255, 0.1)';
            eqCtx.lineWidth = 1.5;
            eqCtx.shadowBlur = 8;
            eqCtx.shadowColor = 'rgba(0, 229, 255, 0.2)';
            eqCtx.beginPath();
            // Extend drawing range beyond visible area to prevent clipping at boundaries
            const extendedStart = -graphW * 0.2; // Start 20% before visible area
            const extendedEnd = graphW * 1.2; // End 20% after visible area
            for (let px = extendedStart; px <= extendedEnd; px += 2) {
                const freq = xToFreq(px, graphW);
                const gain = calculateSummedResponse(mainBands, freq);
                const x = padding.left + px;
                const y = padding.top + dbToY(gain, graphH);
                if (px === extendedStart) eqCtx.moveTo(x, y);
                else eqCtx.lineTo(x, y);
            }
            eqCtx.stroke();
            
            // Second pass - middle shimmer
            eqCtx.strokeStyle = 'rgba(0, 229, 255, 0.25)';
            eqCtx.lineWidth = 1.5;
            eqCtx.shadowBlur = 4;
            eqCtx.beginPath();
            for (let px = extendedStart; px <= extendedEnd; px += 2) {
                const freq = xToFreq(px, graphW);
                const gain = calculateSummedResponse(mainBands, freq);
                const x = padding.left + px;
                const y = padding.top + dbToY(gain, graphH);
                if (px === extendedStart) eqCtx.moveTo(x, y);
                else eqCtx.lineTo(x, y);
            }
            eqCtx.stroke();
            
            // Third pass - core line (ultra thin)
            eqCtx.strokeStyle = 'rgba(0, 229, 255, 0.85)';
            eqCtx.lineWidth = 0.35;
            eqCtx.shadowBlur = 2;
            eqCtx.shadowColor = 'rgba(0, 229, 255, 0.4)';
            eqCtx.beginPath();
            for (let px = extendedStart; px <= extendedEnd; px += 2) {
                const freq = xToFreq(px, graphW);
                const gain = calculateSummedResponse(mainBands, freq);
                const x = padding.left + px;
                const y = padding.top + dbToY(gain, graphH);
                if (px === extendedStart) eqCtx.moveTo(x, y);
                else eqCtx.lineTo(x, y);
            }
            eqCtx.stroke();
            eqCtx.shadowBlur = 0;
            
            // Fill under curve with moving shimmer light (diagonal, one-directional)
            const time = Date.now() / 1000; // Time in seconds
            
            // Create a diagonal gradient that moves continuously in one direction
            const shimmerSpeed = 0.25; // Speed of the shimmer movement
            const shimmerPosition = (time * shimmerSpeed) % 1.5; // 0 to 1.5, loops seamlessly
            
            // Normalize to -0.5 to 1.0 range for smooth entry and exit
            const shimmerCenter = shimmerPosition - 0.25;
            
            // Create diagonal gradient (45 degree angle from bottom-left to top-right)
            const angle = Math.PI / 6; // 30 degrees for subtle diagonal
            const gradientLength = Math.sqrt(Math.pow(w - padding.left - padding.right, 2) + Math.pow(h - padding.top - padding.bottom, 2));
            const gradX1 = padding.left - Math.cos(angle) * gradientLength * 0.5;
            const gradY1 = h - padding.bottom + Math.sin(angle) * gradientLength * 0.5;
            const gradX2 = padding.left + Math.cos(angle) * gradientLength * 1.5;
            const gradY2 = h - padding.bottom - Math.sin(angle) * gradientLength * 1.5;
            
            const gradH = eqCtx.createLinearGradient(gradX1, gradY1, gradX2, gradY2);
            
            const baseOpacity = 0.05;
            const shimmerWidth = 0.08; // Even thinner (was 0.15)
            const shimmerIntensity = 0.15;
            
            // Calculate gradient stops for moving shimmer with softer edges
            const shimmerStart = Math.max(0, shimmerCenter - shimmerWidth);
            const shimmerPeak = shimmerCenter;
            const shimmerEnd = Math.min(1, shimmerCenter + shimmerWidth);
            
            // Add gradient stops for smooth, thin beam
            gradH.addColorStop(0, `rgba(0, 229, 255, ${baseOpacity})`);
            if (shimmerStart > 0 && shimmerStart < 1) {
                gradH.addColorStop(shimmerStart, `rgba(0, 229, 255, ${baseOpacity})`);
            }
            // Soft fade in
            const fadeInPos = shimmerCenter - shimmerWidth * 0.5;
            if (fadeInPos > 0 && fadeInPos < 1) {
                gradH.addColorStop(fadeInPos, `rgba(0, 229, 255, ${baseOpacity + shimmerIntensity * 0.5})`);
            }
            // Peak
            if (shimmerPeak > 0 && shimmerPeak < 1) {
                gradH.addColorStop(shimmerPeak, `rgba(0, 229, 255, ${baseOpacity + shimmerIntensity})`);
            }
            // Soft fade out
            const fadeOutPos = shimmerCenter + shimmerWidth * 0.5;
            if (fadeOutPos > 0 && fadeOutPos < 1) {
                gradH.addColorStop(fadeOutPos, `rgba(0, 229, 255, ${baseOpacity + shimmerIntensity * 0.5})`);
            }
            if (shimmerEnd > 0 && shimmerEnd < 1) {
                gradH.addColorStop(shimmerEnd, `rgba(0, 229, 255, ${baseOpacity})`);
            }
            gradH.addColorStop(1, `rgba(0, 229, 255, ${baseOpacity})`);
            
            // Also create vertical gradient for depth
            const gradV = eqCtx.createLinearGradient(0, padding.top, 0, h - padding.bottom);
            gradV.addColorStop(0, 'rgba(0, 229, 255, 0.15)');
            gradV.addColorStop(0.6, 'rgba(0, 229, 255, 0.05)');
            gradV.addColorStop(1, 'rgba(0, 229, 255, 0.0)');
            
            // Combine both gradients by drawing twice with different composite modes
            eqCtx.save();
            
            // First pass: vertical gradient for base fill
            eqCtx.fillStyle = gradV;
            eqCtx.fill();
            
            // Second pass: diagonal shimmer with lighter composite mode and blur
            eqCtx.shadowBlur = 20; // Increased blur for thinner beam
            eqCtx.shadowColor = 'rgba(0, 229, 255, 0.25)';
            eqCtx.globalCompositeOperation = 'lighter';
            eqCtx.fillStyle = gradH;
            eqCtx.fill();
            
            eqCtx.restore();
        }
        
        // Coordinate conversions
        function freqToX(freq, width) { return width * Math.log10(freq / 20) / 3; }
        function xToFreq(x, width) { return 20 * Math.pow(10, (x / width) * 3); }
        function dbToY(db, height) {
            const range = analyzerScale * 2;
            const center = height / 2;
            return center - (db / (range/2)) * (height / 2);
        }
        function spectrumDbToY(db, height) {
            // Map -100dB to height (bottom) and 0dB to 0 (top)
            const minDb = -100;
            const maxDb = 0;
            const range = maxDb - minDb;
            return height - ((db - minDb) / range) * height;
        }
        function yToDb(y, height) {
            const range = analyzerScale * 2;
            const center = height / 2;
            return ((center - y) / (height / 2)) * (range / 2);
        }
        
        // Update band node positions
        function updateBandNodes() {
            const container = document.getElementById('eq-nodes-container');
            if (!container || !eqCanvas) return;
            
            const w = eqCanvas.width / (window.devicePixelRatio || 1);
            const h = eqCanvas.height / (window.devicePixelRatio || 1);
            const padding = { left: 40, right: 10, top: 50, bottom: 60 };
            const graphW = w - padding.left - padding.right;
            const graphH = h - padding.top - padding.bottom;
            
            container.innerHTML = '';
            
            eqBands.forEach((band, i) => {
                if (!band.active) return;
                
                const x = padding.left + freqToX(band.freq, graphW);
                const y = padding.top + dbToY(band.gain, graphH);
                
                const node = document.createElement('div');
                node.className = 'eq-node' + (band.bypass ? ' bypassed' : '') + (band.dyn && band.dyn.active ? ' dynamic' : '') + (selectedBandIndex === i ? ' selected' : '');
                node.style.left = x + 'px';
                node.style.top = y + 'px';
                node.textContent = i + 1;
                node.dataset.band = i;
                
                // Apply band-specific color
                const bc = getBandColor(i);
                node.style.borderColor = 'rgba(' + bc.r + ', ' + bc.g + ', ' + bc.b + ', 0.8)';
                node.style.color = 'rgb(' + bc.r + ', ' + bc.g + ', ' + bc.b + ')';
                if (selectedBandIndex === i) {
                    node.style.background = 'rgba(' + bc.r + ', ' + bc.g + ', ' + bc.b + ', 0.25)';
                    node.style.boxShadow = '0 0 12px rgba(' + bc.r + ', ' + bc.g + ', ' + bc.b + ', 0.5)';
                }
                
                // Drag handlers
                let dragStarted = false;
                node.addEventListener('mousedown', (e) => {
                    selectedBandIndex = i;
                    showBandPanel(i); // Update UI immediately
                    dragStarted = false;
                    startNodeDrag(e, i, () => { dragStarted = true; });
                });
                node.addEventListener('click', (e) => { 
                    e.stopPropagation(); 
                    if (!dragStarted) {
                        // Single click: Activate if bypassed
                        if (eqBands[i].bypass) {
                            eqBands[i].bypass = false;
                            drawEQSpectrum();
                            updateBandNodes();
                            updateBandPanel();
                            sendEQUpdate(i);
                        }
                    }
                    selectBand(i);
                });
                
                node.addEventListener('dblclick', (e) => {
                    e.stopPropagation();
                    // Double click: Bypass if active
                    if (!eqBands[i].bypass) {
                        eqBands[i].bypass = true;
                        drawEQSpectrum();
                        updateBandNodes();
                        updateBandPanel();
                        sendEQUpdate(i);
                    }
                });
                
                container.appendChild(node);
            });
        }
        
        // Node dragging
        function startNodeDrag(e, bandIndex, onDragStart) {
            e.preventDefault();
            isDraggingNode = true;
            selectedBandIndex = bandIndex;
            let hasMoved = false;
            
            const startX = e.clientX;
            const startY = e.clientY;
            const dragThreshold = 3; // pixels
            
            const onMove = (e) => {
                if (!isDraggingNode) return;
                
                // Check threshold before starting drag
                if (!hasMoved) {
                    const dist = Math.sqrt(Math.pow(e.clientX - startX, 2) + Math.pow(e.clientY - startY, 2));
                    if (dist < dragThreshold) return;
                    
                    // Threshold exceeded, start actual drag
                    if (onDragStart) onDragStart();
                    hasMoved = true;
                }
                
                const rect = eqCanvas.getBoundingClientRect();
                const padding = { left: 40, right: 10, top: 50, bottom: 30 };
                const graphW = rect.width - padding.left - padding.right;
                const graphH = rect.height - padding.top - padding.bottom;
                
                let x = e.clientX - rect.left - padding.left;
                let y = e.clientY - rect.top - padding.top;
                
                x = Math.max(0, Math.min(graphW, x));
                y = Math.max(0, Math.min(graphH, y));
                
                eqBands[bandIndex].freq = Math.round(xToFreq(x, graphW));
                
                // Notch Filter Logic: Drag Y controls Q instead of Gain
                if (eqBands[bandIndex].type === 'notch') {
                    // Map Y position to Q (inverse relationship: higher Y = lower Q, dragging UP increases Q)
                    // Range: 0.1 to 30.0
                    // Use exponential mapping for natural feel
                    const normalizedY = 1.0 - (y / graphH); // 0.0 (bottom) to 1.0 (top)
                    const minQ = 0.1;
                    const maxQ = 30.0;
                    // Exponential mapping: Q = minQ * (maxQ/minQ)^normalizedY
                    const newQ = minQ * Math.pow(maxQ / minQ, normalizedY);
                    
                    eqBands[bandIndex].q = Math.round(newQ * 100) / 100;
                    eqBands[bandIndex].gain = 0; // Keep gain at 0
                } else {
                    eqBands[bandIndex].gain = Math.round(yToDb(y, graphH) * 10) / 10;
                }
                
                drawEQSpectrum();
                updateBandNodes();
                updateBandPanel();
                sendEQUpdate(bandIndex);
            };
            
            const onWheel = (e) => {
                if (!isDraggingNode) return;
                e.preventDefault();
                // Scroll up = increase Q, scroll down = decrease Q
                const delta = e.deltaY > 0 ? -0.1 : 0.1;
                const currentQ = eqBands[bandIndex].q;
                const newQ = Math.max(0.1, Math.min(10, currentQ + delta * currentQ * 0.5));
                eqBands[bandIndex].q = Math.round(newQ * 100) / 100;
                
                drawEQSpectrum();
                updateBandNodes();
                updateBandPanel();
                sendEQUpdate(bandIndex);
            };
            
            const onUp = () => {
                isDraggingNode = false;
                document.removeEventListener('mousemove', onMove);
                document.removeEventListener('mouseup', onUp);
                document.removeEventListener('wheel', onWheel);
            };
            
            document.addEventListener('mousemove', onMove);
            document.addEventListener('mouseup', onUp);
            document.addEventListener('wheel', onWheel, { passive: false });
        }
        
        // Canvas interactions
        function onCanvasClick(e) {
            if (isDraggingNode) return;
            
            const rect = eqCanvas.getBoundingClientRect();
            const padding = { left: 40, right: 10, top: 50, bottom: 30 };
            const graphW = rect.width - padding.left - padding.right;
            const graphH = rect.height - padding.top - padding.bottom;
            
            const clickX = e.clientX - rect.left - padding.left;
            const clickY = e.clientY - rect.top - padding.top;
            
            // Check if click is within graph bounds
            if (clickX < 0 || clickX > graphW || clickY < 0 || clickY > graphH) {
                closeBandPanel();
                selectedBandIndex = -1;
                updateBandNodes();
                return;
            }
            
            // Check if clicking on an existing band node
            const clickedBandIndex = findBandNearClick(clickX, clickY, graphW, graphH);
            if (clickedBandIndex !== -1) {
                // Don't add new band, just select this one
                selectBand(clickedBandIndex);
                return;
            }
            
            // Check if clicking near the EQ curve
            const clickFreq = xToFreq(clickX, graphW);
            const clickGain = yToDb(clickY, graphH);
            const curveGain = calculateTotalGainAtFreq(clickFreq);
            const curveY = dbToY(curveGain, graphH);
            const distanceToCurve = Math.abs(clickY - curveY);
            
            // If within 30px of the curve, add a new band
            if (distanceToCurve < 30) {
                addBandAt(clickFreq, clickGain);
                return;
            }
            
            // Otherwise deselect
            closeBandPanel();
            selectedBandIndex = -1;
            updateBandNodes();
        }
        
        // Find if there's a band near the click position
        function findBandNearClick(clickX, clickY, graphW, graphH) {
            const threshold = 20; // pixels
            
            for (let i = 0; i < eqBands.length; i++) {
                const band = eqBands[i];
                if (!band.active) continue;
                
                const bandX = freqToX(band.freq, graphW);
                const bandY = dbToY(band.gain, graphH);
                
                const dist = Math.sqrt(Math.pow(clickX - bandX, 2) + Math.pow(clickY - bandY, 2));
                if (dist < threshold) {
                    return i;
                }
            }
            return -1;
        }
        
        // Helper for Hybrid Summing (Parallel Boosts, Series Cuts)
        function calculateSummedResponse(bands, freq) {
            let posMag = 1.0;
            let negDb = 0;
            
            for (let i = 0; i < bands.length; i++) {
                const b = bands[i];
                if (!b.active || b.bypass) continue;
                
                const db = calculateBandResponse(b, freq);
                if (db > 0) {
                    posMag += (Math.pow(10, db/20) - 1);
                    hasBoost = true;
                } else {
                    negDb += db;
                }
            }
            
            const boostDb = hasBoost ? 20 * Math.log10(posMag) : 0;
            return boostDb + negDb;
        }

        // Calculate total EQ gain at a specific frequency
        function calculateTotalGainAtFreq(freq) {
            return calculateSummedResponse(eqBands, freq);
        }
        
        function onCanvasDoubleClick(e) {
            // Double-click also adds a band (legacy behavior)
            const rect = eqCanvas.getBoundingClientRect();
            const padding = { left: 40, right: 10, top: 50, bottom: 30 };
            const graphW = rect.width - padding.left - padding.right;
            const graphH = rect.height - padding.top - padding.bottom;
            
            const x = e.clientX - rect.left - padding.left;
            const y = e.clientY - rect.top - padding.top;
            
            if (x < 0 || x > graphW || y < 0 || y > graphH) return;
            
            // Check if clicking on existing band
            const clickedBandIndex = findBandNearClick(x, y, graphW, graphH);
            if (clickedBandIndex !== -1) return; // Don't add if on existing band
            
            addBandAt(xToFreq(x, graphW), yToDb(y, graphH));
        }
        
        // Add band
        function addEQBand() {
            const inactive = eqBands.findIndex(b => !b.active);
            if (inactive === -1) return;
            
            eqBands[inactive].active = true;
            eqBands[inactive].freq = 1000;
            eqBands[inactive].gain = 0;
            eqBands[inactive].type = currentToolMode;
            
            selectBand(inactive);
            sendEQUpdate(inactive);
        }
        
        function addBandAt(freq, gain) {
            const inactive = eqBands.findIndex(b => !b.active);
            if (inactive === -1) return;
            
            eqBands[inactive].active = true;
            eqBands[inactive].freq = Math.round(freq);
            eqBands[inactive].gain = Math.round(gain * 10) / 10;
            eqBands[inactive].type = currentToolMode;
            
            drawEQSpectrum();
            updateBandNodes();
            selectBand(inactive);
            sendEQUpdate(inactive);
        }
        
        // Select band
        function selectBand(index) {
            selectedBandIndex = index;
            updateBandNodes();
            showBandPanel(index);
        }
        
        // Band panel
        function showBandPanel(index) {
            const panel = document.getElementById('eq-band-panel');
            if (!panel) return;
            
            const band = eqBands[index];
            
            // Initialize stereoMode and slope if not exists
            if (!band.stereoMode) band.stereoMode = 'stereo';
            if (!band.slope) band.slope = 12;
            
            // Update dropdowns to show current state
            updateTypeDropdown(band.type);
            updateStereoDropdown(band.stereoMode);
            updateSlopeDropdown(band.slope);
            
            updateBandPanel();
            panel.classList.add('active');
            
            // Setup dropdown item clicks
            document.querySelectorAll('#bp-type-menu .eq-dropdown-item').forEach(item => {
                item.onclick = () => {
                    if (selectedBandIndex < 0 || !eqBands[selectedBandIndex]) return;
                    const index = selectedBandIndex; // Use global index
                    const newType = item.dataset.type;
                    const band = eqBands[index];
                    
                    // Handle Notch Gain Logic (Save/Restore)
                    if (newType === 'notch' && band.type !== 'notch') {
                        band.prevGain = band.gain; // Save current gain
                        band.gain = 0;
                    } else if (newType !== 'notch' && band.type === 'notch') {
                        if (band.prevGain !== undefined) {
                            band.gain = band.prevGain; // Restore gain
                        }
                    }
                    
                    band.type = newType;
                    updateTypeDropdown(newType);
                    closeTypeDropdown();
                    drawEQSpectrum();
                    updateBandPanel();
                    updateBandNodes();
                    sendEQUpdate(index);
                };
            });
            
            // Setup stereo mode and slope dropdowns
            initStereoDropdown();
            initSlopeDropdown();
            
            // Setup rotary knobs
            setupKnob('bp-freq-knob', 20, 20000, band.freq, (v) => {
                if (selectedBandIndex < 0) return;
                eqBands[selectedBandIndex].freq = Math.round(v);
                updateBandPanel();
                drawEQSpectrum();
                updateBandNodes();
                sendEQUpdate(selectedBandIndex);
            }, true);
            
            setupKnob('bp-gain-knob', -18, 18, band.gain, (v) => {
                if (selectedBandIndex < 0) return;
                // Prevent gain change if notch
                if (eqBands[selectedBandIndex].type === 'notch') return;
                
                eqBands[selectedBandIndex].gain = Math.round(v * 10) / 10;
                updateBandPanel();
                drawEQSpectrum();
                updateBandNodes();
                sendEQUpdate(selectedBandIndex);
            });
            
            setupKnob('bp-q-knob', 0.1, 10, band.q, (v) => {
                if (selectedBandIndex < 0) return;
                eqBands[selectedBandIndex].q = Math.round(v * 100) / 100;
                updateBandPanel();
                drawEQSpectrum();
                sendEQUpdate(selectedBandIndex);
            });
            
            setupKnob('bp-dyngain-knob', -18, 18, band.dyn.range, (v) => {
                if (selectedBandIndex < 0) return;
                const roundedVal = Math.round(v * 10) / 10;
                eqBands[selectedBandIndex].dyn.range = roundedVal;
                
                if (roundedVal !== 0) {
                    eqBands[selectedBandIndex].dyn.active = true;
                    if (!eqBands[selectedBandIndex].dynMode) eqBands[selectedBandIndex].dynMode = 'dynamic';
                } else {
                    eqBands[selectedBandIndex].dyn.active = false;
                }
                
                // Update visualization array
                dynamicGainReductions[selectedBandIndex] = roundedVal;
                
                updateBandPanel();
                drawEQSpectrum();
                sendEQUpdate(selectedBandIndex);
            });
            

            
            // Initial knob positions
            updateKnobPosition('bp-freq-knob', 20, 20000, band.freq, true);
            updateKnobPosition('bp-gain-knob', -18, 18, band.gain);
            updateKnobPosition('bp-q-knob', 0.1, 10, band.q);
            
            // Update power buttons (on = active state)
            const bypassBtn = document.getElementById('bp-bypass');
            if (bypassBtn) bypassBtn.classList.toggle('on', !band.bypass); // on = not bypassed
            
            // Dynamic Gain knob setup
            const dynKnobGroup = document.querySelector('.eq-knob-group.dynamic-knob');
            const dynGainValue = band.dyn.range || 0;
            
            // Initialize dyn.range if not exists
            if (band.dyn.range === undefined) band.dyn.range = 0;
            
            // Show/hide dynamic gain knob based on dynamic active state
            if (dynKnobGroup) {
                dynKnobGroup.classList.toggle('active', band.dyn.active);
            }
            

            
            updateKnobPosition('bp-dyngain-knob', -18, 18, band.dyn.range);
            document.getElementById('bp-dyngain-val').textContent = (band.dyn.range >= 0 ? '+' : '') + band.dyn.range.toFixed(1) + ' dB';
        }
        
        function updateBandPanel() {
            if (selectedBandIndex < 0) return;
            const band = eqBands[selectedBandIndex];
            
            document.getElementById('bp-freq-val').textContent = formatFreq(band.freq);
            document.getElementById('bp-gain-val').textContent = (band.gain >= 0 ? '+' : '') + band.gain.toFixed(1) + ' dB';
            document.getElementById('bp-q-val').textContent = band.q.toFixed(2);
            
            // Update knob positions
            updateKnobPosition('bp-freq-knob', 20, 20000, band.freq, true);
            
            // Handle Notch filter gain (disable and show 0dB)
            const gainKnob = document.getElementById('bp-gain-knob');
            const gainVal = document.getElementById('bp-gain-val');
            const gainGroup = gainKnob.closest('.eq-knob-group');
            
            if (band.type === 'notch') {
                updateKnobPosition('bp-gain-knob', -18, 18, 0); // Visual zero
                gainVal.textContent = "0.0 dB";
                if (gainGroup) {
                    gainGroup.style.opacity = '0.4';
                    gainGroup.style.pointerEvents = 'none';
                }
            } else {
                updateKnobPosition('bp-gain-knob', -18, 18, band.gain);
                if (gainGroup) {
                    gainGroup.style.opacity = '1';
                    gainGroup.style.pointerEvents = 'auto';
                }
            }
            
            updateKnobPosition('bp-q-knob', 0.1, 10, band.q);
            
            // Update Dynamic Gain knob
            if (band.dyn.range === undefined) band.dyn.range = 0;
            updateKnobPosition('bp-dyngain-knob', -18, 18, band.dyn.range);
            const dynValEl = document.getElementById('bp-dyngain-val');
            if (dynValEl) dynValEl.textContent = (band.dyn.range >= 0 ? '+' : '') + band.dyn.range.toFixed(1) + ' dB';
            
            // Update dynamic knob visibility - active when value is non-zero
            const dynKnobGroup = document.querySelector('.eq-knob-group.dynamic-knob');
            if (dynKnobGroup) dynKnobGroup.classList.toggle('active', band.dyn.range !== 0);
        }
        
        function closeBandPanel() {
            const panel = document.getElementById('eq-band-panel');
            if (panel) panel.classList.remove('active');
            closeTypeDropdown();
        }
        
        // Dropdown functions
        function toggleTypeDropdown() {
            const dropdown = document.getElementById('bp-type-dropdown');
            if (dropdown) dropdown.classList.toggle('open');
        }
        
        function closeTypeDropdown() {
            const dropdown = document.getElementById('bp-type-dropdown');
            if (dropdown) dropdown.classList.remove('open');
        }
        
        // Stereo Mode Dropdown
        function toggleStereoDropdown() {
            closeAllDropdowns('stereo');
            const dropdown = document.getElementById('bp-stereo-dropdown');
            if (dropdown) dropdown.classList.toggle('open');
        }
        
        function closeStereoDropdown() {
            const dropdown = document.getElementById('bp-stereo-dropdown');
            if (dropdown) dropdown.classList.remove('open');
        }
        
        const stereoModeNames = {
            'stereo': 'Stereo',
            'mid': 'Mid',
            'side': 'Side',
            'left': 'Left',
            'right': 'Right'
        };
        
        function updateStereoDropdown(mode) {
            const label = document.getElementById('bp-stereo-label');
            const trigger = document.getElementById('bp-stereo-trigger');
            
            if (label) label.textContent = stereoModeNames[mode] || 'Stereo';
            
            // Update trigger styling based on mode
            if (trigger) {
                trigger.classList.remove('mid', 'side', 'left', 'right');
                if (mode !== 'stereo') trigger.classList.add(mode);
            }
            
            // Update active state in menu
            document.querySelectorAll('#bp-stereo-menu .eq-stereo-item').forEach(item => {
                item.classList.toggle('active', item.dataset.mode === mode);
            });
        }
        
        function initStereoDropdown() {
            document.querySelectorAll('#bp-stereo-menu .eq-stereo-item').forEach(item => {
                item.onclick = () => {
                    const newMode = item.dataset.mode;
                    if (selectedBandIndex >= 0 && eqBands[selectedBandIndex]) {
                        eqBands[selectedBandIndex].stereoMode = newMode;
                        updateStereoDropdown(newMode);
                        closeStereoDropdown();
                        drawEQSpectrum();
                        updateBandNodes();
                        sendStereoModeUpdate(selectedBandIndex, newMode);
                    }
                };
            });
        }
        
        function sendStereoModeUpdate(index, mode) {
            window.location = 'sphere://eq/stereo/' + index + '/' + mode;
        }
        
        // Slope Dropdown
        function toggleSlopeDropdown() {
            closeAllDropdowns('slope');
            const dropdown = document.getElementById('bp-slope-dropdown');
            if (dropdown) dropdown.classList.toggle('open');
        }
        
        function closeSlopeDropdown() {
            const dropdown = document.getElementById('bp-slope-dropdown');
            if (dropdown) dropdown.classList.remove('open');
        }
        
        function updateSlopeDropdown(slope) {
            const label = document.getElementById('bp-slope-label');
            if (label) label.textContent = slope + 'dB';
            
            // Update active state in menu
            document.querySelectorAll('#bp-slope-menu .eq-slope-item').forEach(item => {
                item.classList.toggle('active', item.dataset.slope === String(slope));
            });
        }
        
        function initSlopeDropdown() {
            document.querySelectorAll('#bp-slope-menu .eq-slope-item').forEach(item => {
                item.onclick = () => {
                    const newSlope = parseInt(item.dataset.slope);
                    if (selectedBandIndex >= 0 && eqBands[selectedBandIndex]) {
                        eqBands[selectedBandIndex].slope = newSlope;
                        updateSlopeDropdown(newSlope);
                        closeSlopeDropdown();
                        drawEQSpectrum();
                        sendSlopeUpdate(selectedBandIndex, newSlope);
                    }
                };
            });
        }
        
        function sendSlopeUpdate(index, slope) {
            window.location = 'sphere://eq/slope/' + index + '/' + slope;
        }
        
        function closeAllDropdowns(except) {
            if (except !== 'type') closeTypeDropdown();
            if (except !== 'stereo') closeStereoDropdown();
            if (except !== 'slope') closeSlopeDropdown();
        }
        
        const typeNames = {
            'bell': 'Bell',
            'lowshelf': 'Low Shelf',
            'highshelf': 'High Shelf',
            'lowcut': 'Low Cut',
            'highcut': 'High Cut',
            'notch': 'Notch'
        };
        
        const typeIcons = {
            'bell': 'M2 14 Q12 2 22 14',
            'lowshelf': 'M2 6 L8 6 Q12 6 14 12 L22 12',
            'highshelf': 'M2 12 L10 12 Q12 12 14 6 L22 6',
            'lowcut': 'M2 16 Q6 16 10 9 L22 9',
            'highcut': 'M2 9 L14 9 Q18 9 22 16',
            'notch': 'M2 6 L8 6 Q12 16 16 6 L22 6'
        };
        
        function updateTypeDropdown(type) {
            const label = document.getElementById('bp-type-label');
            const icon = document.getElementById('bp-type-icon');
            
            // Get band color for icon
            const bandColor = selectedBandIndex >= 0 ? getBandColor(selectedBandIndex) : { r: 0, g: 229, b: 255 };
            
            if (label) label.textContent = typeNames[type] || 'Bell';
            if (icon) {
                const path = icon.querySelector('path');
                if (path) path.setAttribute('d', typeIcons[type] || typeIcons['bell']);
                // Set icon color to band color
                icon.style.stroke = 'rgb(' + bandColor.r + ', ' + bandColor.g + ', ' + bandColor.b + ')';
            }
            
            // Update active state in menu
            document.querySelectorAll('#bp-type-menu .eq-dropdown-item').forEach(item => {
                item.classList.toggle('active', item.dataset.type === type);
            });
        }
        
        // Close dropdown when clicking outside
        document.addEventListener('click', (e) => {
            const dropdown = document.getElementById('bp-type-dropdown');
            if (dropdown && !dropdown.contains(e.target)) {
                closeTypeDropdown();
            }
        });
        
        // Rotary Knob setup
        let activeKnob = null;
        let knobStartY = 0;
        let knobStartValue = 0;
        
        function setupKnob(id, min, max, value, onChange, isLog = false) {
            const knob = document.getElementById(id);
            if (!knob) return;
            
            knob._min = min;
            knob._max = max;
            knob._isLog = isLog;
            knob._onChange = onChange;
            knob._value = value; // Store current value on element
            
            knob.onmousedown = (e) => {
                e.preventDefault();
                activeKnob = knob;
                knobStartY = e.clientY;
                // Use the stored _value from the element, not the captured closure value
                const currentValue = knob._value;
                knobStartValue = isLog ? Math.log(currentValue / min) / Math.log(max / min) : (currentValue - min) / (max - min);
                document.addEventListener('mousemove', onKnobDrag);
                document.addEventListener('mouseup', onKnobRelease);
            };
        }
        
        function onKnobDrag(e) {
            if (!activeKnob) return;
            const sensitivity = 200;
            const deltaY = knobStartY - e.clientY;
            let pct = knobStartValue + deltaY / sensitivity;
            pct = Math.max(0, Math.min(1, pct));
            
            let val;
            if (activeKnob._isLog) {
                val = activeKnob._min * Math.pow(activeKnob._max / activeKnob._min, pct);
            } else {
                val = activeKnob._min + (activeKnob._max - activeKnob._min) * pct;
            }
            
            activeKnob._value = val; // Store current value for next drag
            activeKnob._onChange(val);
            updateKnobVisual(activeKnob, pct);
        }
        
        function onKnobRelease() {
            activeKnob = null;
            document.removeEventListener('mousemove', onKnobDrag);
            document.removeEventListener('mouseup', onKnobRelease);
        }
        
        function updateKnobVisual(knobOrId, pct) {
            const knob = typeof knobOrId === 'string' ? document.getElementById(knobOrId) : knobOrId;
            if (!knob) return;
            
            // Update indicator rotation (-135deg to 135deg)
            const angle = -135 + pct * 270;
            knob.style.setProperty('--knob-rotation', angle + 'deg');
            if (knob.querySelector) {
                const before = knob;
                before.style.transform = 'rotate(' + angle + 'deg)';
            }
            
            // Update shimmer arc
            const shimmerId = knob.id.replace('-knob', '-shimmer');
            const shimmer = document.getElementById(shimmerId);
            if (shimmer) {
                const isBipolar = knob.id.includes('gain'); // gain and dyngain knobs are bipolar (center = 0)
                const isDynamic = knob.classList.contains('dynamic');
                
                // Get color: dynamic = yellow, others = band color
                let color;
                if (isDynamic) {
                    color = 'rgba(255, 200, 0, 0.6)';
                } else if (selectedBandIndex >= 0) {
                    const bc = getBandColor(selectedBandIndex);
                    color = 'rgba(' + bc.r + ', ' + bc.g + ', ' + bc.b + ', 0.6)';
                } else {
                    color = 'rgba(0, 229, 255, 0.6)';
                }
                
                if (isBipolar) {
                    // For bipolar knobs: shimmer from center (0 dB at 12 o'clock) to current position
                    const centerAngle = 135;
                    const currentAngle = pct * 270;
                    
                    let arcGradient;
                    if (pct >= 0.5) {
                        const arcStart = centerAngle;
                        const arcEnd = currentAngle + 10; // +10 for visual overlap
                        arcGradient = 'conic-gradient(from 225deg, transparent 0deg ' + arcStart + 'deg, black ' + arcStart + 'deg ' + arcEnd + 'deg, transparent ' + arcEnd + 'deg 360deg)';
                    } else {
                        const arcStart = currentAngle;
                        const arcEnd = centerAngle;
                        arcGradient = 'conic-gradient(from 225deg, transparent 0deg ' + arcStart + 'deg, black ' + arcStart + 'deg ' + arcEnd + 'deg, transparent ' + arcEnd + 'deg 360deg)';
                    }
                    
                    // Apply mask to define the arc shape
                    shimmer.style.webkitMaskImage = arcGradient;
                    shimmer.style.maskImage = arcGradient;
                    // Apply color to the background
                    shimmer.style.background = color;
                    
                } else {
                    // Unipolar: start to current
                    const endAngle = pct * 270;
                    const arcGradient = 'conic-gradient(from 225deg, black 0deg ' + endAngle + 'deg, transparent ' + endAngle + 'deg 360deg)';
                    
                    shimmer.style.webkitMaskImage = arcGradient;
                    shimmer.style.maskImage = arcGradient;
                    shimmer.style.background = color;
                }
            }
        }
        
        function updateKnobPosition(id, min, max, value, isLog = false) {
            const knob = document.getElementById(id);
            if (!knob) return;
            
            // Store current value for drag tracking
            knob._value = value;
            knob._min = min;
            knob._max = max;
            knob._isLog = isLog;
            
            let pct;
            if (isLog) {
                pct = Math.log(value / min) / Math.log(max / min);
            } else {
                pct = (value - min) / (max - min);
            }
            pct = Math.max(0, Math.min(1, pct));
            updateKnobVisual(knob, pct);
        }
        
        // Legacy Slider setup (for dynamic section)
        function setupSlider(id, min, max, value, onChange, isLog = false) {
            const track = document.getElementById(id);
            if (!track) return;
            
            const thumb = track.querySelector('.eq-slider-thumb');
            const fill = track.querySelector('.eq-slider-fill');
            
            const onDrag = (e) => {
                const rect = track.getBoundingClientRect();
                let pct = (e.clientX - rect.left) / rect.width;
                pct = Math.max(0, Math.min(1, pct));
                
                let val;
                if (isLog) {
                    val = min * Math.pow(max / min, pct);
                } else {
                    val = min + (max - min) * pct;
                }
                onChange(val);
            };
            
            track.onmousedown = (e) => {
                onDrag(e);
                document.addEventListener('mousemove', onDrag);
                document.addEventListener('mouseup', () => {
                    document.removeEventListener('mousemove', onDrag);
                }, { once: true });
            };
        }
        
        function updateSliderPosition(id, min, max, value, isLog = false) {
            const track = document.getElementById(id);
            if (!track) return;
            
            let pct;
            if (isLog) {
                pct = Math.log(value / min) / Math.log(max / min);
            } else {
                pct = (value - min) / (max - min);
            }
            pct = Math.max(0, Math.min(1, pct)) * 100;
            
            const thumb = track.querySelector('.eq-slider-thumb');
            const fill = track.querySelector('.eq-slider-fill');
            if (thumb) thumb.style.left = pct + '%';
            if (fill) fill.style.width = pct + '%';
        }
        
        // Toggle functions
        function toggleBandBypass() {
            if (selectedBandIndex < 0) return;
            eqBands[selectedBandIndex].bypass = !eqBands[selectedBandIndex].bypass;
            // Note: 'on' means band is active (NOT bypassed), so we invert
            document.getElementById('bp-bypass').classList.toggle('on', !eqBands[selectedBandIndex].bypass);
            drawEQSpectrum();
            updateBandNodes();
            sendEQUpdate(selectedBandIndex);
        }
        
        function toggleBandSolo() {
            // Solo implementation
        }
        
        function toggleBandDynamic() {
            if (selectedBandIndex < 0) return;
            eqBands[selectedBandIndex].dyn.active = !eqBands[selectedBandIndex].dyn.active;
            eqBands[selectedBandIndex].dyn.mode = eqBands[selectedBandIndex].dyn.active ? 'compress' : 'off';
            
            // Update dynamic gain knob visibility
            const dynKnobGroup = document.querySelector('.eq-knob-group.dynamic-knob');
            if (dynKnobGroup) {
                dynKnobGroup.classList.toggle('active', eqBands[selectedBandIndex].dyn.active);
            }
            
            if (eqBands[selectedBandIndex].dyn.active) {
                dynamicGainReductions[selectedBandIndex] = eqBands[selectedBandIndex].dyn.range || 0;
            } else {
                dynamicGainReductions[selectedBandIndex] = 0;
            }
            
            drawEQSpectrum();
            updateBandNodes();
            sendDynUpdate(selectedBandIndex);
        }
        
        // Delete band function
        function deleteBand() {
            if (selectedBandIndex < 0) return;
            
            eqBands[selectedBandIndex].active = false;
            eqBands[selectedBandIndex].gain = 0;
            eqBands[selectedBandIndex].dyn.active = false;
            dynamicGainReductions[selectedBandIndex] = 0;
            
            closeBandPanel();
            selectedBandIndex = -1;
            drawEQSpectrum();
            updateBandNodes();
            
            // Send update to backend
            sendEQUpdate(selectedBandIndex);
        }
        
        // Tool mode
        function setToolMode(mode) {
            currentToolMode = mode;
            document.querySelectorAll('.eq-tool-btn').forEach(b => b.classList.remove('active'));
            event.target.classList.add('active');
        }
        
        // Phase mode
        let currentPhaseMode = 0;
        const phaseModes = ['ZERO LAT', 'NATURAL', 'LINEAR'];
        function cyclePhaseMode() {
            currentPhaseMode = (currentPhaseMode + 1) % 3;
            document.getElementById('eq-phase-btn').textContent = phaseModes[currentPhaseMode];
            const modes = ['minimum', 'natural', 'linear'];
            window.location = 'sphere://eq/phasemode/' + modes[currentPhaseMode];
        }
        
        function toggleAnalyzer() {
            analyzerEnabled = !analyzerEnabled;
            const btn = document.getElementById('eq-analyzer-btn');
            if (analyzerEnabled) {
                btn.classList.add('active');
            } else {
                btn.classList.remove('active');
            }
            drawEQSpectrum(); // Redraw to show/hide analyzer
        }
        
        // Helpers
        function formatFreq(f) { return f >= 1000 ? (f/1000).toFixed(f >= 10000 ? 1 : 2) + ' kHz' : Math.round(f) + ' Hz'; }
        
        function calculateBandResponse(b, freq) {
            const ratio = freq / b.freq;
            const logRatio = Math.log2(ratio);
            const q = b.q || 1;
            const slope = b.slope || 12; // Use band slope, default 12dB/oct
            
            switch(b.type) {
                case 'bell': {
                    // Super-Gaussian for "squarer" shape with higher slope
                    // 12dB -> exp 2 (Gaussian), 24dB -> exp 4, 36dB -> exp 6
                    const exponent = Math.max(2, slope / 6);
                    return b.gain * Math.exp(-Math.pow(Math.abs(logRatio * q), exponent) * 2);
                }
                    
                case 'lowshelf': {
                    // Steeper transition with higher slope
                    // 12dB -> factor 4, 24dB -> factor 8, 36dB -> factor 12
                    const knee = 1 / (q * 0.5 + 0.5);
                    const steepness = slope / 3;
                    const transition = 1 / (1 + Math.exp(logRatio * steepness / knee));
                    return b.gain * transition;
                }
                    
                case 'highshelf': {
                    // Steeper transition with higher slope
                    const knee = 1 / (q * 0.5 + 0.5);
                    const steepness = slope / 3;
                    const transition = 1 / (1 + Math.exp(-logRatio * steepness / knee));
                    return b.gain * transition;
                }
                    
                case 'lowcut': {
                    // Butterworth filter response for smooth knee
                    // Gain = 10 * log10(1 / (1 + (f/fc)^(2n)))
                    // 2n = slope / 3 (since slope is dB/oct, 6dB/oct is order 1 -> power 2)
                    const power = slope / 3;
                    const magnitudeSquared = 1 / (1 + Math.pow(ratio, power));
                    return 10 * Math.log10(magnitudeSquared + 1e-20); // Avoid log(0)
                }
                    
                case 'highcut': {
                    // Butterworth filter response for smooth knee
                    // Gain = 10 * log10(1 / (1 + (fc/f)^(2n)))
                    const power = slope / 3;
                    const magnitudeSquared = 1 / (1 + Math.pow(1 / ratio, power));
                    return 10 * Math.log10(magnitudeSquared + 1e-20);
                }
                    
                case 'notch': 
                    // Sharper notch with higher slope
                    const exponent = Math.max(2, slope / 6);
                    return -60 * Math.exp(-Math.pow(Math.abs(logRatio * q * 3), exponent));
                    
                default: return 0;
            }
        }
        
        // Backend communication
        function sendEQUpdate(i) {
            const b = eqBands[i];
            window.location = 'sphere://eq/band/' + i + '/' + (b.active && !b.bypass ? 1 : 0) + '/' + b.type + '/' + b.freq + '/' + b.gain + '/' + b.q;
            if (b.dyn.active) sendDynUpdate(i);
        }
        
        function sendDynUpdate(i) {
            const d = eqBands[i].dyn;
            window.location = 'sphere://eq/dynamic/' + i + '/' + d.mode + '/' + d.thresh + '/' + d.ratio + '/' + d.attack + '/' + d.release + '/6';
        }
        
        function toggleEQMaster() { eqEnabled = !eqEnabled; window.location = 'sphere://eq/enable/' + (eqEnabled ? '1' : '0'); }
        
        // Character Mode Logic
        let currentCharacterMode = 0; // 0=Clean, 1=Subtle, 2=Warm
        const charModeNames = ['CLEAN', 'SUBTLE', 'WARM'];
        
        function cycleCharacterMode() {
            currentCharacterMode = (currentCharacterMode + 1) % 3;
            const btn = document.getElementById('eq-char-btn');
            if (btn) btn.textContent = charModeNames[currentCharacterMode];
            
            // Send to backend: sphere://eq/character/MODE
            window.location = 'sphere://eq/character/' + currentCharacterMode;
        }
        function initEQMiniCanvas() {
            const miniCanvas = document.getElementById('eq-mini-canvas');
            if (!miniCanvas) return;
            const miniCtx = miniCanvas.getContext('2d');
            const rect = miniCanvas.getBoundingClientRect();
            const dpr = window.devicePixelRatio || 1;
            miniCanvas.width = rect.width * dpr;
            miniCanvas.height = rect.height * dpr;
            miniCtx.scale(dpr, dpr);
            const w = rect.width, h = rect.height;
            
            miniCtx.clearRect(0, 0, w, h);
            
            // Background
            miniCtx.fillStyle = 'rgba(0, 0, 0, 0.35)';
            miniCtx.fillRect(0, 0, w, h);
            
            // Center line
            miniCtx.strokeStyle = 'rgba(0, 229, 255, 0.12)';
            miniCtx.lineWidth = 0.5;
            miniCtx.beginPath();
            miniCtx.moveTo(0, h/2);
            miniCtx.lineTo(w, h/2);
            miniCtx.stroke();
            
            // EQ curve
            miniCtx.strokeStyle = 'rgba(0, 229, 255, 0.8)';
            miniCtx.lineWidth = 1;
            miniCtx.beginPath();
            for (let x = 0; x < w; x++) {
                const freq = 20 * Math.pow(1000, x / w);
                let totalGain = 0;
                eqBands.forEach(b => {
                    if (b.active && !b.bypass) totalGain += calculateBandResponse(b, freq);
                });
                const y = h/2 - (totalGain / 24) * (h/2);
                if (x === 0) miniCtx.moveTo(x, y);
                else miniCtx.lineTo(x, y);
            }
            miniCtx.stroke();
        }
    )JS";

inline const juce::String html =
    R"HTML(<!DOCTYPE html><html><head><meta charset="UTF-8"><meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate"><meta http-equiv="Pragma" content="no-cache"><meta http-equiv="Expires" content="0"><style>)HTML" +
    css + R"HTML(</style></head><body>
      <div class="top-nav">
        <button class="settings-btn" onclick="toggleSettings()">Settings</button>
        <div class="nav-center">
          <button class="view-btn active" data-view="synth" onclick="switchView('synth')">Synth</button>
          <button class="view-btn" data-view="fx" onclick="switchView('fx')">FX</button>
        </div>
        <button class="tutorial-btn" onclick="toggleTutorial()">?</button>
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
        <!-- Full-screen spectrum analyzer -->
        <div class="eq-spectrum">
          <canvas id="eq-spectrum-canvas" class="eq-spectrum-canvas"></canvas>
        </div>
        
        <!-- Grid and labels -->
        <div class="eq-grid" id="eq-grid"></div>
        <!-- Bottom Bar Container -->
        <div class="eq-bottom-bar">
            <!-- Left Controls: Scale & Oversampling -->
            <div class="eq-controls-left">
              <!-- Scale Dropdown -->
              <div class="eq-dropdown" id="eq-scale-dropdown">
                <button class="eq-output-btn" onclick="toggleScaleDropdown()">SCALE: <span id="eq-scale-label">12dB</span></button>
                <div class="eq-dropdown-menu" id="eq-scale-menu">
                  <button class="eq-dropdown-item" onclick="setScale(3)">3 dB</button>
                  <button class="eq-dropdown-item" onclick="setScale(6)">6 dB</button>
                  <button class="eq-dropdown-item active" onclick="setScale(12)">12 dB</button>
                  <button class="eq-dropdown-item" onclick="setScale(24)">24 dB</button>
                  <button class="eq-dropdown-item" onclick="setScale(36)">36 dB</button>
                </div>
              </div>

              <!-- Oversampling Dropdown -->
              <div class="eq-dropdown" id="eq-oversampling-dropdown">
                <button class="eq-output-btn" onclick="toggleOversamplingDropdown()">OS: <span id="eq-oversampling-label">OFF</span></button>
                <div class="eq-dropdown-menu" id="eq-oversampling-menu">
                  <button class="eq-dropdown-item active" onclick="setOversampling(0)">OFF</button>
                  <button class="eq-dropdown-item" onclick="setOversampling(2)">2x</button>
                  <button class="eq-dropdown-item" onclick="setOversampling(4)">4x</button>
                </div>
              </div>
            </div>

            <!-- Right Controls: Phase, Warmth, Analyzer -->
            <div class="eq-controls-right">
              <button class="eq-output-btn active" id="eq-phase-btn" onclick="cyclePhaseMode()">ZERO LAT</button>
              <button class="eq-output-btn" id="eq-char-btn" onclick="cycleCharacterMode()">CLEAN</button>
              <button class="eq-output-btn active" id="eq-analyzer-btn" onclick="toggleAnalyzer()">ANALYZER</button>
            </div>
        </div>
        
        <!-- Toolbar (top-center) -->
        <!-- DEBUG: v2.1 Ultra Thin UI -->
        <div class="eq-toolbar">
          <button class="eq-tool-btn" onclick="setToolMode('bell')" title="Bell">●</button>
          <button class="eq-tool-btn" onclick="setToolMode('lowshelf')" title="Low Shelf">◢</button>
          <button class="eq-tool-btn" onclick="setToolMode('highshelf')" title="High Shelf">◣</button>
          <div class="eq-tool-divider"></div>
          <button class="eq-tool-btn" onclick="setToolMode('lowcut')" title="Low Cut">╱</button>
          <button class="eq-tool-btn" onclick="setToolMode('highcut')" title="High Cut">╲</button>
          <div class="eq-tool-divider"></div>
          <button class="eq-tool-btn" onclick="setToolMode('notch')" title="Notch">▽</button>
        </div>
        
        <!-- Minimize button (top-right) -->
 
        <button class="eq-close" onclick="closeEQ()">−</button>
        
        <!-- Version indicator for debugging -->
        <!-- Version text removed -->
        
        <!-- Band nodes container -->
        <div id="eq-nodes-container"></div>
        
        <!-- Add band button -->
        <button class="eq-add-band" onclick="addEQBand()">+</button>
        
        <!-- Floating band control panel - Modern Apple-style design -->
        <div class="eq-band-panel" id="eq-band-panel">
          <!-- Top row: Power, Curve Type Dropdown, Delete -->
          <div class="eq-bp-top-row">
            <button class="eq-power-btn" id="bp-bypass" onclick="toggleBandBypass()" title="Band On/Off">
              <svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="3"/><path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83"/></svg>
            </button>
            
            <div class="eq-type-dropdown" id="bp-type-dropdown">
              <button class="eq-dropdown-trigger" id="bp-type-trigger" onclick="toggleTypeDropdown()">
                <svg class="eq-dropdown-icon" id="bp-type-icon" viewBox="0 0 24 18"><path d="M2 14 Q12 2 22 14"/></svg>
                <span class="eq-dropdown-label" id="bp-type-label">Bell</span>
                <svg class="eq-dropdown-arrow" viewBox="0 0 12 12"><path d="M3 4.5 L6 7.5 L9 4.5"/></svg>
              </button>
              <div class="eq-dropdown-menu" id="bp-type-menu">
                <button class="eq-dropdown-item active" data-type="bell"><svg viewBox="0 0 24 18"><path d="M2 14 Q12 2 22 14"/></svg><span>Bell</span></button>
                <button class="eq-dropdown-item" data-type="lowshelf"><svg viewBox="0 0 24 18"><path d="M2 6 L8 6 Q12 6 14 12 L22 12"/></svg><span>Low Shelf</span></button>
                <button class="eq-dropdown-item" data-type="highshelf"><svg viewBox="0 0 24 18"><path d="M2 12 L10 12 Q12 12 14 6 L22 6"/></svg><span>High Shelf</span></button>
                <button class="eq-dropdown-item" data-type="lowcut"><svg viewBox="0 0 24 18"><path d="M2 16 Q6 16 10 9 L22 9"/></svg><span>Low Cut</span></button>
                <button class="eq-dropdown-item" data-type="highcut"><svg viewBox="0 0 24 18"><path d="M2 9 L14 9 Q18 9 22 16"/></svg><span>High Cut</span></button>
                <button class="eq-dropdown-item" data-type="notch"><svg viewBox="0 0 24 18"><path d="M2 6 L8 6 Q12 16 16 6 L22 6"/></svg><span>Notch</span></button>
              </div>
            </div>
            
            <!-- Stereo Mode Dropdown -->
            <div class="eq-stereo-dropdown" id="bp-stereo-dropdown">
              <button class="eq-stereo-trigger" id="bp-stereo-trigger" onclick="toggleStereoDropdown()">
                <span id="bp-stereo-label">Stereo</span>
                <svg class="eq-stereo-arrow" viewBox="0 0 12 12"><path d="M3 4.5 L6 7.5 L9 4.5"/></svg>
              </button>
              <div class="eq-stereo-menu" id="bp-stereo-menu">
                <button class="eq-stereo-item active" data-mode="stereo">Stereo</button>
                <button class="eq-stereo-item" data-mode="mid">Mid</button>
                <button class="eq-stereo-item" data-mode="side">Side</button>
                <button class="eq-stereo-item" data-mode="left">Left</button>
                <button class="eq-stereo-item" data-mode="right">Right</button>
              </div>
            </div>
            
            <!-- Slope Dropdown -->
            <div class="eq-slope-dropdown" id="bp-slope-dropdown">
              <button class="eq-slope-trigger" id="bp-slope-trigger" onclick="toggleSlopeDropdown()">
                <span id="bp-slope-label">12dB</span>
                <svg class="eq-slope-arrow" viewBox="0 0 12 12"><path d="M3 4.5 L6 7.5 L9 4.5"/></svg>
              </button>
              <div class="eq-slope-menu" id="bp-slope-menu">
                <button class="eq-slope-item active" data-slope="12">12 dB/oct</button>
                <button class="eq-slope-item" data-slope="24">24 dB/oct</button>
                <button class="eq-slope-item" data-slope="36">36 dB/oct</button>
              </div>
            </div>
            
            <button class="eq-delete-btn" id="bp-delete" onclick="deleteBand()" title="Delete Band">
              <svg viewBox="0 0 24 24"><path d="M6 6l12 12M6 18L18 6"/></svg>
            </button>
          </div>
          
          <!-- Knobs row -->
          <div class="eq-knobs-row">
            <!-- Old controls removed -->
            <div class="eq-knob-group">
              <span class="eq-knob-label">Freq</span>
              <div class="eq-knob-container">
                <div class="eq-knob-shimmer" id="bp-freq-shimmer"></div>
                <div class="eq-knob" id="bp-freq-knob" data-param="freq"></div>
              </div>
              <span class="eq-knob-value" id="bp-freq-val">1 kHz</span>
            </div>
            
            <div class="eq-knob-group">
              <span class="eq-knob-label">Gain</span>
              <div class="eq-knob-container">
                <div class="eq-knob-shimmer" id="bp-gain-shimmer"></div>
                <div class="eq-knob" id="bp-gain-knob" data-param="gain"></div>
              </div>
              <span class="eq-knob-value" id="bp-gain-val">0 dB</span>
            </div>
            
            <div class="eq-knob-group">
              <span class="eq-knob-label">Q</span>
              <div class="eq-knob-container">
                <div class="eq-knob-shimmer" id="bp-q-shimmer"></div>
                <div class="eq-knob" id="bp-q-knob" data-param="q"></div>
              </div>
              <span class="eq-knob-value" id="bp-q-val">1.00</span>
            </div>
            
            <div class="eq-knob-group dynamic-knob">
              <span class="eq-knob-label dynamic">Dyn</span>
              <div class="eq-knob-container">
                <div class="eq-knob-shimmer dynamic" id="bp-dyngain-shimmer"></div>
                <div class="eq-knob dynamic" id="bp-dyngain-knob" data-param="dyngain"></div>
              </div>
              <span class="eq-knob-value dynamic" id="bp-dyngain-val">0 dB</span>
            </div>
          </div>
        </div>
      </div>
      <div class="volume-meter-container"><div class="volume-meter-bar" id="volume-bar"></div></div>
      <div id="popup-sphere" class="tutorial-popup"><div class="popup-title">Sphere Visualizer</div><div class="popup-text">This sphere reacts to your notes.</div><button class="popup-dismiss" onclick="dismissPopup('popup-sphere')">Next</button></div>
      <div id="popup-controls" class="tutorial-popup"><div class="popup-title">Sound Selection</div><div class="popup-text">Choose different waveforms.</div><button class="popup-dismiss" onclick="dismissPopup('popup-controls')">Next</button></div>
      <div id="popup-keyboard" class="tutorial-popup"><div class="popup-title">Piano Keyboard</div><div class="popup-text">Use A-J keys to play C3-B3, K-; for C4-E4. Press Z/X to shift octave down/up.</div><button class="popup-dismiss" onclick="dismissPopup('popup-keyboard')">Got It!</button></div>
      <div id="settings-modal" class="tutorial-popup" style="max-width: 400px; top: 50%; left: 50%; transform: translate(-50%, -50%);">
        <div class="popup-title">Settings</div>
        <div style="margin-bottom: 15px;"><label style="display: block; color: rgba(255,255,255,0.6); font-size: 11px; margin-bottom: 5px; text-transform: uppercase;">Audio Output</label><select id="audio-output" onchange="setAudioOutput(this.value)" class="settings-select"><option>Loading...</option></select></div>
        <div style="margin-bottom: 20px;"><label style="display: block; color: rgba(255,255,255,0.6); font-size: 11px; margin-bottom: 5px; text-transform: uppercase;">MIDI Input</label><select id="midi-input" onchange="setMidiInput(this.value)" class="settings-select"><option>Loading...</option></select></div>
        <button class="popup-dismiss" onclick="toggleSettings()">Close</button>
      </div>
    </body><script>)HTML" +
    js + R"HTML(</script></html>)HTML";
} // namespace SphereSynthResources
