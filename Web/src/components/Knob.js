class Knob {
    constructor(container, label, options = {}) {
        this.container = typeof container === 'string' ? document.getElementById(container) : container;
        this.label = label;
        this.min = options.min !== undefined ? options.min : 0;
        this.max = options.max !== undefined ? options.max : 100;
        this.value = options.value !== undefined ? options.value : this.min;
        this.step = options.step !== undefined ? options.step : 1;
        this.units = options.units || '';
        this.size = options.size || 60; // Default size increased
        this.onChange = options.onChange || (() => { });
        this.color = options.color || '#00e5ff';

        this.isDragging = false;
        this.lastY = 0;

        this.render();
        this.attachEvents();
    }

    render() {
        if (!this.container) return;

        const center = this.size / 2;
        const radius = this.size * 0.4; // Slightly smaller to fit stroke
        const strokeWidth = this.size * 0.08;

        // Angles
        const startAngle = -Math.PI * 0.75;
        const endAngle = Math.PI * 0.75;
        const rangeAngle = endAngle - startAngle;

        const percentage = (this.value - this.min) / (this.max - this.min);
        const currentAngle = startAngle + (percentage * rangeAngle);

        // Calculate paths
        // Background track (full arc)
        const bgParams = this.describeArc(center, center, radius, startAngle, endAngle);
        // Value track (active arc)
        const valParams = this.describeArc(center, center, radius, startAngle, currentAngle);

        // Marker position on the cap
        const capRadius = radius * 0.75; // Inner rotating cap
        const markerX = center + (capRadius - 5) * Math.cos(currentAngle);
        const markerY = center + (capRadius - 5) * Math.sin(currentAngle);
        const markerStartX = center + (capRadius * 0.2) * Math.cos(currentAngle); // Start slightly from center
        const markerStartY = center + (capRadius * 0.2) * Math.sin(currentAngle);

        this.container.innerHTML = `
            <div class="knob-wrapper" style="display: flex; flex-direction: column; align-items: center; gap: 8px; pointer-events: none;">
                <div class="knob-circle" style="position: relative; width: ${this.size}px; height: ${this.size}px; pointer-events: auto; cursor: ns-resize;">
                    <svg width="${this.size}" height="${this.size}" viewBox="0 0 ${this.size} ${this.size}">
                        <defs>
                            <linearGradient id="knobGradient" x1="0%" y1="0%" x2="0%" y2="100%">
                                <stop offset="0%" style="stop-color:#3a3a40;stop-opacity:1" />
                                <stop offset="100%" style="stop-color:#2a2a30;stop-opacity:1" />
                            </linearGradient>
                            <filter id="dropshadow" height="130%">
                                <feGaussianBlur in="SourceAlpha" stdDeviation="2"/>
                                <feOffset dx="0" dy="2" result="offsetblur"/>
                                <feFlood flood-color="rgba(0,0,0,0.5)"/>
                                <feComposite in2="offsetblur" operator="in"/>
                                <feMerge> 
                                    <feMergeNode/>
                                    <feMergeNode in="SourceGraphic"/>
                                </feMerge>
                            </filter>
                        </defs>
                        
                        <!-- Track Background -->
                        <path d="${bgParams}" fill="none" stroke="#1a1a1f" stroke-width="${strokeWidth}" stroke-linecap="round"/>
                        
                        <!-- Value Arc (Glow) -->
                        <path d="${valParams}" fill="none" stroke="${this.color}" stroke-width="${strokeWidth}" stroke-linecap="round" 
                              style="filter: drop-shadow(0 0 3px ${this.color}); opacity: 0.8;" />
                        
                        <!-- Inner Cap (Rotating visually via marker, but actually static circle background) -->
                        <!-- Detailed Cap Circle -->
                        <circle cx="${center}" cy="${center}" r="${capRadius}" fill="url(#knobGradient)" stroke="#111" stroke-width="1" filter="url(#dropshadow)" />
                        
                        <!-- Marker Line on Cap -->
                        <line x1="${markerStartX}" y1="${markerStartY}" x2="${markerX}" y2="${markerY}" stroke="${this.color}" stroke-width="2" stroke-linecap="round" />
                    </svg>
                    
                    <!-- Value Display (Centered) -->
                    <!-- Removed: Center value, using label instead or external. Actually user wants generic style, usually value is below. -->
                </div>
                <!-- Label & Value -->
                <div class="knob-info" style="text-align: center; line-height: 1.2;">
                    <div class="knob-value" style="font-family: 'Segoe UI', sans-serif; font-size: 11px; color: ${this.color}; font-weight: 600;">${this.formatValue(this.value)} ${this.units}</div>
                    <div class="knob-label" style="font-size: 10px; text-transform: uppercase; color: #888; font-weight: 500; letter-spacing: 0.5px;">${this.label}</div>
                </div>
            </div>
        `;

        this.knobElement = this.container.querySelector('.knob-circle');
    }

    describeArc(x, y, radius, startAngle, endAngle) {
        const start = this.polarToCartesian(x, y, radius, endAngle);
        const end = this.polarToCartesian(x, y, radius, startAngle);
        const largeArcFlag = endAngle - startAngle <= Math.PI ? "0" : "1";
        return [
            "M", start.x, start.y,
            "A", radius, radius, 0, largeArcFlag, 0, end.x, end.y
        ].join(" ");
    }

    polarToCartesian(centerX, centerY, radius, angleInRadians) {
        return {
            x: centerX + (radius * Math.cos(angleInRadians)),
            y: centerY + (radius * Math.sin(angleInRadians))
        };
    }

    attachEvents() {
        if (!this.knobElement) return;

        this.knobElement.addEventListener('mousedown', (e) => {
            this.isDragging = true;
            this.lastY = e.clientY;
            document.body.style.cursor = 'ns-resize';
            window.addEventListener('mousemove', this.handleMouseMove);
            window.addEventListener('mouseup', this.handleMouseUp);
        });
    }

    handleMouseMove = (e) => {
        if (!this.isDragging) return;

        const deltaY = this.lastY - e.clientY;
        this.lastY = e.clientY;

        const range = this.max - this.min;
        const sensitivity = 200;
        const deltaValue = (deltaY / sensitivity) * range;

        let newValue = this.value + deltaValue;
        newValue = Math.max(this.min, Math.min(this.max, newValue));

        if (newValue !== this.value) {
            this.setValue(newValue);
            this.onChange(this.value);
        }
    }

    handleMouseUp = () => {
        this.isDragging = false;
        document.body.style.cursor = '';
        window.removeEventListener('mousemove', this.handleMouseMove);
        window.removeEventListener('mouseup', this.handleMouseUp);
    }

    setValue(val) {
        this.value = val;
        this.render();
        this.attachEvents();
    }

    formatValue(val) {
        // Smart formatting
        if (this.step < 1) return val.toFixed(1);
        if (Math.abs(val) < 10 && this.step % 1 !== 0) return val.toFixed(1);
        return Math.round(val);
    }
}
