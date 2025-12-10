// Compressor Module Logic
// Depends on Knob.js being loaded

let compKnobs = {};
let compDeltaEnabled = false;

function initCompressorUI() {
    console.log("Initializing Pro Compressor UI...");

    const controlStrip = document.getElementById('comp-controls');
    if (!controlStrip) return;

    controlStrip.innerHTML = '';
    // Main container style (can be moved to CSS)
    controlStrip.className = 'comp-pro-controls';

    // 1. Threshold Section (Large on Left)
    const threshSection = document.createElement('div');
    threshSection.className = 'comp-section comp-section-thresh';
    controlStrip.appendChild(threshSection);

    const threshWrapper = document.createElement('div');
    threshWrapper.id = 'knob-container-threshold';
    threshSection.appendChild(threshWrapper);

    compKnobs['threshold'] = new Knob(threshWrapper, 'Threshold', {
        min: -60, max: 0, value: compSettings.threshold, step: 0.1, units: 'dB', size: 90, color: '#00e5ff',
        onChange: (val) => {
            compSettings.threshold = val;
            window.location = `sphere://comp/threshold/${val}`;
            drawTransferCurve();
        }
    });

    // 2. Main Parameters (Ratio, Attack, Release)
    const mainSection = document.createElement('div');
    mainSection.className = 'comp-section comp-section-main';
    controlStrip.appendChild(mainSection);

    const mainParams = [
        { id: 'ratio', label: 'Ratio', min: 1, max: 20, val: compSettings.ratio, step: 0.1, units: ':1', size: 65 },
        { id: 'attack', label: 'Attack', min: 0.1, max: 500, val: compSettings.attack, step: 0.1, units: 'ms', size: 65 },
        { id: 'release', label: 'Release', min: 10, max: 3000, val: compSettings.release, step: 1, units: 'ms', size: 65 }
    ];

    mainParams.forEach(p => {
        const w = document.createElement('div');
        w.id = `knob-container-${p.id}`;
        mainSection.appendChild(w);
        compKnobs[p.id] = new Knob(w, p.label, {
            min: p.min, max: p.max, value: p.val, step: p.step, units: p.units, size: p.size, color: '#ffb74d', // Orange/Gold for timing/ratio
            onChange: (val) => {
                compSettings[p.id] = val;
                window.location = `sphere://comp/${p.id}/${val}`;
                if (p.id === 'ratio') drawTransferCurve();
            }
        });
    });

    // 3. Secondary Parameters (Knee, Makeup) + Delta
    const secSection = document.createElement('div');
    secSection.className = 'comp-section comp-section-sec';
    controlStrip.appendChild(secSection);

    const secParams = [
        { id: 'knee', label: 'Knee', min: 0, max: 24, val: compSettings.knee, step: 0.1, units: 'dB', size: 50 },
        { id: 'makeup', label: 'Gain', min: -12, max: 24, val: compSettings.makeup, step: 0.1, units: 'dB', size: 50 }
    ];

    secParams.forEach(p => {
        const w = document.createElement('div');
        w.id = `knob-container-${p.id}`;
        secSection.appendChild(w);
        compKnobs[p.id] = new Knob(w, p.label, {
            min: p.min, max: p.max, value: p.val, step: p.step, units: p.units, size: p.size, color: '#aaa', // Neutral for util
            onChange: (val) => {
                compSettings[p.id] = val;
                window.location = `sphere://comp/${p.id}/${val}`;
                if (p.id === 'knee') drawTransferCurve();
            }
        });
    });

    // Delta Button (Small, near secondary controls)
    const deltaWrapper = document.createElement('div');
    deltaWrapper.className = 'comp-delta-wrapper';
    deltaWrapper.innerHTML = `
        <div class="comp-delta-btn ${compDeltaEnabled ? 'active' : ''}" onclick="toggleDelta()">
            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M3 18v-6a9 9 0 0 1 18 0v6"></path><path d="M21 19a2 2 0 0 1-2 2h-1a2 2 0 0 1-2-2v-3a2 2 0 0 1 2-2h3zM3 19a2 2 0 0 0 2 2h1a2 2 0 0 0 2-2v-3a2 2 0 0 0-2-2H3z"></path></svg>
        </div>
        <div class="knob-label">DELTA</div>
     `;
    secSection.appendChild(deltaWrapper);

    initCompCanvases();
}

function toggleDelta() {
    compDeltaEnabled = !compDeltaEnabled;
    const btn = document.querySelector('.comp-delta-btn');
    if (btn) btn.classList.toggle('active', compDeltaEnabled);
    window.location = `sphere://comp/delta/${compDeltaEnabled ? 1 : 0}`;
}

// Update Knob externally (e.g. from Transfer Curve drag)
function updateKnob(param, value) {
    if (compKnobs[param]) {
        compKnobs[param].setValue(value);
    }
}
