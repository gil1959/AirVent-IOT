// AirVent Dashboard Logic (MQTT & Simulation Mode)

const CONFIG = {
    mqttBroker: "broker.emqx.io",
    mqttPort: 8084, // WebSocket SSL port for EMQX
    mqttPath: "/mqtt",
    topicTelemetry: "airvent/telemetry",
    topicControl: "airvent/fan/control",
    topicStatus: "airvent/fan/status",
    tempThreshold: 30.0,
    aqiThreshold: 350
};

let isSimulationMode = true;
let mqttClient = null;
let simulationInterval = null;

// State Data
let state = {
    temperature: 28.5,
    humidity: 62.0,
    gasPpm: 180,
    fanState: "OFF",
    fanMode: "AUTO",
    reason: "Kondisi Ruangan Normal"
};

// ==================== INISIALISASI GRAFIK (CHART.JS) ====================
const maxDataPoints = 15;
const labels = Array.from({ length: maxDataPoints }, (_, i) => `-${maxDataPoints - i}s`);

// Chart Suhu & Kelembaban
const ctxTempHum = document.getElementById("tempHumChart").getContext("2d");
const tempHumChart = new Chart(ctxTempHum, {
    type: "line",
    data: {
        labels: [...labels],
        datasets: [
            {
                label: "Suhu (°C)",
                data: Array(maxDataPoints).fill(28.5),
                borderColor: "#f43f5e",
                backgroundColor: "rgba(244, 63, 94, 0.1)",
                tension: 0.35,
                fill: true,
                yAxisID: "yTemp"
            },
            {
                label: "Kelembaban (%)",
                data: Array(maxDataPoints).fill(62),
                borderColor: "#38bdf8",
                backgroundColor: "transparent",
                borderDash: [5, 5],
                tension: 0.35,
                yAxisID: "yHum"
            }
        ]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: { labels: { color: "#94a3b8", font: { size: 11 } } }
        },
        scales: {
            x: { grid: { color: "rgba(255,255,255,0.05)" }, ticks: { color: "#64748b" } },
            yTemp: {
                type: "linear",
                position: "left",
                min: 20,
                max: 45,
                grid: { color: "rgba(255,255,255,0.05)" },
                ticks: { color: "#f43f5e" }
            },
            yHum: {
                type: "linear",
                position: "right",
                min: 30,
                max: 100,
                grid: { drawOnChartArea: false },
                ticks: { color: "#38bdf8" }
            }
        }
    }
});

// Chart Kualitas Udara (Gas PPM)
const ctxAqi = document.getElementById("aqiChart").getContext("2d");
const aqiChart = new Chart(ctxAqi, {
    type: "line",
    data: {
        labels: [...labels],
        datasets: [
            {
                label: "Kadar Gas (PPM)",
                data: Array(maxDataPoints).fill(180),
                borderColor: "#fbbf24",
                backgroundColor: "rgba(251, 191, 36, 0.15)",
                tension: 0.35,
                fill: true
            }
        ]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: { labels: { color: "#94a3b8", font: { size: 11 } } }
        },
        scales: {
            x: { grid: { color: "rgba(255,255,255,0.05)" }, ticks: { color: "#64748b" } },
            y: {
                min: 50,
                max: 600,
                grid: { color: "rgba(255,255,255,0.05)" },
                ticks: { color: "#fbbf24" }
            }
        }
    }
});

// ==================== UPDATE TAMPILAN DASHBOARD ====================
function updateUI() {
    document.getElementById("val-temperature").innerText = state.temperature.toFixed(1);
    document.getElementById("val-humidity").innerText = state.humidity.toFixed(1);
    document.getElementById("val-gas-ppm").innerText = Math.round(state.gasPpm);
    document.getElementById("val-fan-state").innerText = state.fanState;
    document.getElementById("val-fan-mode").innerText = state.fanMode;
    document.getElementById("val-fan-reason").innerText = state.reason;

    // Badge Suhu
    const tempBadge = document.getElementById("temp-badge");
    if (state.temperature >= CONFIG.tempThreshold) {
        tempBadge.className = "px-2 py-0.5 rounded bg-rose-950 text-rose-400 border border-rose-800 text-[11px] font-medium";
        tempBadge.innerText = "Tinggi";
    } else {
        tempBadge.className = "px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 border border-emerald-800 text-[11px] font-medium";
        tempBadge.innerText = "Normal";
    }

    // Badge Udara
    const airBadge = document.getElementById("air-badge");
    if (state.gasPpm >= CONFIG.aqiThreshold) {
        airBadge.className = "px-2 py-0.5 rounded bg-rose-950 text-rose-400 border border-rose-800 text-[11px] font-medium";
        airBadge.innerText = "Buruk";
    } else if (state.gasPpm >= 220) {
        airBadge.className = "px-2 py-0.5 rounded bg-amber-950 text-amber-400 border border-amber-800 text-[11px] font-medium";
        airBadge.innerText = "Sedang";
    } else {
        airBadge.className = "px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 border border-emerald-800 text-[11px] font-medium";
        airBadge.innerText = "Baik";
    }

    // Badge Fan
    const fanBadge = document.getElementById("fan-status-badge");
    const fanIcon = document.querySelector(".animate-spin-slow");

    if (state.fanState === "ON") {
        fanBadge.className = "px-4 py-2 rounded-xl text-sm font-semibold flex items-center gap-2 bg-emerald-950/60 text-emerald-400 border border-emerald-700 shadow-lg shadow-emerald-950/30";
        if (fanIcon) fanIcon.classList.add("fan-spinning");
    } else {
        fanBadge.className = "px-4 py-2 rounded-xl text-sm font-semibold flex items-center gap-2 bg-slate-800 text-slate-400 border border-slate-700";
        if (fanIcon) fanIcon.classList.remove("fan-spinning");
    }

    // Tombol Mode Active Style
    const btnAuto = document.getElementById("btn-mode-auto");
    const btnOn = document.getElementById("btn-fan-on");
    const btnOff = document.getElementById("btn-fan-off");

    if (state.fanMode === "AUTO") {
        btnAuto.className = "flex-1 py-1.5 px-3 rounded-lg text-xs font-semibold bg-cyan-600 text-white shadow-lg shadow-cyan-600/20 border border-cyan-500 transition";
        btnOn.className = "py-1.5 px-3 rounded-lg text-xs font-medium bg-slate-800 hover:bg-slate-700 text-slate-300 border border-slate-700 transition";
        btnOff.className = "py-1.5 px-3 rounded-lg text-xs font-medium bg-slate-800 hover:bg-slate-700 text-slate-300 border border-slate-700 transition";
    } else {
        btnAuto.className = "flex-1 py-1.5 px-3 rounded-lg text-xs font-medium bg-slate-800 hover:bg-slate-700 text-slate-300 border border-slate-700 transition";
        if (state.fanState === "ON") {
            btnOn.className = "py-1.5 px-3 rounded-lg text-xs font-semibold bg-emerald-600 text-white border border-emerald-500 transition";
            btnOff.className = "py-1.5 px-3 rounded-lg text-xs font-medium bg-slate-800 hover:bg-slate-700 text-slate-300 border border-slate-700 transition";
        } else {
            btnOn.className = "py-1.5 px-3 rounded-lg text-xs font-medium bg-slate-800 hover:bg-slate-700 text-slate-300 border border-slate-700 transition";
            btnOff.className = "py-1.5 px-3 rounded-lg text-xs font-semibold bg-rose-600 text-white border border-rose-500 transition";
        }
    }

    // Update Data ke Charts
    pushChartData(tempHumChart, [state.temperature, state.humidity]);
    pushChartData(aqiChart, [state.gasPpm]);
}

function pushChartData(chart, values) {
    chart.data.labels.shift();
    chart.data.labels.push(new Date().toLocaleTimeString().split(" ")[0]);

    values.forEach((val, idx) => {
        chart.data.datasets[idx].data.shift();
        chart.data.datasets[idx].data.push(val);
    });

    chart.update("none");
}

// ==================== MODE SIMULASI LOKAL ====================
function startSimulation() {
    if (simulationInterval) clearInterval(simulationInterval);

    simulationInterval = setInterval(() => {
        // Variasi fluktuasi data acak
        const tempDelta = (Math.random() - 0.48) * 0.4;
        const gasDelta = (Math.random() - 0.48) * 15;

        state.temperature = Math.max(22, Math.min(38, state.temperature + tempDelta));
        state.humidity = Math.max(40, Math.min(90, state.humidity + (Math.random() - 0.5) * 0.3));
        state.gasPpm = Math.max(80, Math.min(500, state.gasPpm + gasDelta));

        // Evaluasi logika exhaust otomatis di simulasi
        if (state.fanMode === "AUTO") {
            if (state.temperature >= CONFIG.tempThreshold || state.gasPpm >= CONFIG.aqiThreshold) {
                state.fanState = "ON";
                state.reason = "Terpicu Otomatis (Suhu/Gas Melebihi Batas)";
            } else if (state.temperature <= CONFIG.tempThreshold - 1.5 && state.gasPpm <= CONFIG.aqiThreshold - 30) {
                state.fanState = "OFF";
                state.reason = "Kondisi Ruangan Kembali Normal";
            }
        }

        updateUI();
    }, 2000);
}

// ==================== KONTROL BUTTON HANDLERS ====================
function sendCommand(cmd) {
    if (isSimulationMode) {
        if (cmd === "AUTO") {
            state.fanMode = "AUTO";
            state.reason = "Beralih ke Otomatis";
        } else if (cmd === "ON") {
            state.fanMode = "MANUAL";
            state.fanState = "ON";
            state.reason = "Dinyalakan Manual oleh Pengguna";
        } else if (cmd === "OFF") {
            state.fanMode = "MANUAL";
            state.fanState = "OFF";
            state.reason = "Dimatikan Manual oleh Pengguna";
        }
        updateUI();
    } else if (mqttClient && mqttClient.isConnected()) {
        const message = new Paho.MQTT.Message(cmd);
        message.destinationName = CONFIG.topicControl;
        mqttClient.send(message);
        console.log("[MQTT OUT] Perintah terkirim:", cmd);
    }
}

document.getElementById("btn-mode-auto").addEventListener("click", () => sendCommand("AUTO"));
document.getElementById("btn-fan-on").addEventListener("click", () => sendCommand("ON"));
document.getElementById("btn-fan-off").addEventListener("click", () => sendCommand("OFF"));

// Toggle Live MQTT vs Mode Simulasi
document.getElementById("btn-toggle-source").addEventListener("click", () => {
    if (isSimulationMode) {
        connectMqtt();
    } else {
        disconnectMqtt();
    }
});

// ==================== KONEKSI MQTT WEBSOCKET ====================
function connectMqtt() {
    const indicator = document.getElementById("conn-indicator");
    const connText = document.getElementById("conn-text");
    const toggleBtn = document.getElementById("btn-toggle-source");

    connText.innerText = "Menghubungkan ke MQTT...";
    indicator.className = "w-2.5 h-2.5 rounded-full bg-amber-500 animate-ping";

    const clientId = "AirVent-WebDash-" + Math.floor(Math.random() * 10000);
    mqttClient = new Paho.MQTT.Client(CONFIG.mqttBroker, Number(CONFIG.mqttPort), CONFIG.mqttPath, clientId);

    mqttClient.onConnectionLost = (responseObject) => {
        if (responseObject.errorCode !== 0) {
            console.error("[MQTT ERROR] Koneksi terputus:", responseObject.errorMessage);
            switchToSimulationMode("Koneksi MQTT Gagal / Terputus");
        }
    };

    mqttClient.onMessageArrived = (message) => {
        try {
            const data = JSON.parse(message.payloadString);
            state.temperature = data.temperature ?? state.temperature;
            state.humidity = data.humidity ?? state.humidity;
            state.gasPpm = data.gasPpm ?? state.gasPpm;
            state.fanState = data.fanState ?? state.fanState;
            state.fanMode = data.fanMode ?? state.fanMode;
            state.reason = data.reason ?? state.reason;
            updateUI();
        } catch (e) {
            console.warn("[MQTT PARSE ERROR]", e);
        }
    };

    mqttClient.connect({
        useSSL: true,
        timeout: 5,
        onSuccess: () => {
            isSimulationMode = false;
            clearInterval(simulationInterval);

            indicator.className = "w-2.5 h-2.5 rounded-full bg-emerald-500 animate-pulse";
            connText.innerText = "Live MQTT Terhubung";
            toggleBtn.innerText = "Beralih ke Simulasi";

            mqttClient.subscribe(CONFIG.topicTelemetry);
            mqttClient.subscribe(CONFIG.topicStatus);
            console.log("[MQTT] Berhasil terhubung & subscribe topik!");
        },
        onFailure: (err) => {
            console.error("[MQTT FAIL]", err);
            switchToSimulationMode("Gagal Konek Broker MQTT");
        }
    });
}

function disconnectMqtt() {
    if (mqttClient && mqttClient.isConnected()) {
        mqttClient.disconnect();
    }
    switchToSimulationMode("Mode Simulasi");
}

function switchToSimulationMode(label) {
    isSimulationMode = true;
    const indicator = document.getElementById("conn-indicator");
    const connText = document.getElementById("conn-text");
    const toggleBtn = document.getElementById("btn-toggle-source");

    indicator.className = "w-2.5 h-2.5 rounded-full bg-cyan-500 animate-pulse";
    connText.innerText = `${label || "Simulasi Aktif"}`;
    toggleBtn.innerText = "Beralih ke Live MQTT";

    startSimulation();
}

// Inisialisasi awal pada saat halaman dimuat
switchToSimulationMode("Simulasi Aktif");
updateUI();

