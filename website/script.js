const BASE_URL = 'http://127.0.0.1:5000';
//var error_alerted = false;

// Toggle controls available based on which mode is selected
function toggleControls(isAutoMode) {
    var fanSpeedSlider = document.getElementById("fanSpeedSlider");
    var pressureTargetInput = document.getElementById("pressureTargetInput");

    if (isAutoMode) {
        // Auto mode
        fanSpeedSlider.disabled = true;
        pressureTargetInput.disabled = false;
    } else {
        // Manual mode
        fanSpeedSlider.disabled = false;
        pressureTargetInput.disabled = true;
    }
}

function openTab(tabName) {
    var i, tabContent;
    tabContent = document.getElementsByClassName("tab-content");
    for (i = 0; i < tabContent.length; i++) {
        tabContent[i].style.display = "none";
    }
    document.getElementById(tabName).style.display = "block";
    if (tabName === "controlTab") {
        populateStatusSection();
    } else if (tabName === "historyTab") {
        populateHistoryTable();
    } else if (tabName === "logTab") {
        populateLogTable();
    }
}

function populateHistoryTable() {
    const historyTableBody = document.querySelector('#historyTable tbody');
    
    // Clear existing rows
    historyTableBody.innerHTML = '';

    fetch(`${BASE_URL}/api/v0.1/readings/all`)
        .then(response => response.json())
        .then(data => {
            data.forEach(function (entry) {
                var row = document.createElement('tr');
                row.innerHTML = '<td>' + entry.index + '</td>' +
                                '<td>' + entry.timestamp + '</td>' +
                                '<td>' + entry.speed + '</td>' +
                                '<td>' + entry.setpoint + '</td>' +
                                '<td>' + entry.pressure + '</td>' +
                                '<td>' + entry.co2 + '</td>' +
                                '<td>' + entry.rh + '</td>' +
                                '<td>' + entry.temp + '</td>';
                historyTableBody.appendChild(row);
            });
        })
        .catch(error => {
            console.error('Error fetching history data: ', error);
            var row = document.createElement('tr');
            row.innerHTML = '<td colspan="8">Error fetching history data</td>';
            historyTableBody.appendChild(row);
        });
}

function populateLogTable() {
    const logTableBody = document.querySelector('#logTable tbody');

    // Clear existing rows
    logTableBody.innerHTML = '';

    fetch(`${BASE_URL}/api/v0.1/logs/all`)
        .then(response => response.json())
        .then(data => {
            data.forEach(function (entry) {
                var row = document.createElement('tr');
                row.innerHTML = '<td>' + entry.index + '</td>' +
                                '<td>' + entry.message + '</td>' +
                                '<td>' + entry.timestamp + '</td>';
                logTableBody.appendChild(row);
            });
        })
        .catch(error => {
            console.error('Error fetching log data: ', error);
            var row = document.createElement('tr');
            row.innerHTML = '<td colspan="3">Error fetching log data</td>';
            logTableBody.appendChild(row);
        });

}

function populateStatusSection() {
    fetch(`${BASE_URL}/api/v0.1/status`)
        .then(response => response.json())
        .then(data => {
            if (data.auto === 1) {
                document.getElementById('modeStatus').textContent = 'Auto';
            } else {
                document.getElementById('modeStatus').textContent = 'Manual';
            }
            document.getElementById('fanSpeedStatus').textContent = data.speed;
            document.getElementById('setPointStatus').textContent = data.setpoint;
            document.getElementById('pressureStatus').textContent = data.pressure;
            document.getElementById('co2Status').textContent = data.co2;
            document.getElementById('humidityStatus').textContent = data.rh;
            document.getElementById('temperatureStatus').textContent = data.temp;
            /*
            if (data.error === 1) {
                if (!error_alerted) {
                    alert('Error: Unable to reach target set point.');
                    error_alerted = true;
                }
            } else {
                error_alerted = false;
            }*/
        })
        .catch(error => {
            console.error('Error fetching status data: ', error);
            document.getElementById('modeStatus').textContent = 'N/A';
            document.getElementById('fanSpeedStatus').textContent = 'N/A';
            document.getElementById('setPointStatus').textContent = 'N/A';
            document.getElementById('pressureStatus').textContent = 'N/A';
            document.getElementById('co2Status').textContent = 'N/A';
            document.getElementById('humidityStatus').textContent = 'N/A';
            document.getElementById('temperatureStatus').textContent = 'N/A';
        });
}

function applySettings() {
    var mode;
    var value;

    if (document.getElementById('autoMode').checked) {
        mode = 1;
    } else if (document.getElementById('manualMode').checked) {
        mode = 0;
    } else {
        alert('Please select a mode');
        return;
    }

    if (mode === 1) {
        value = document.getElementById('pressureTargetInput').value;
        if (value < 0 || value > 120) {
            alert('Please enter a pressure target value between 0 and 120');
            return;
        }
        console.log(value);
    } else if (mode === 0) {
        value = document.getElementById('fanSpeedSlider').value;
        if (value < 0 || value > 100) {
            alert('Please enter a fan speed value between 0 and 100');
            return;
        }
        console.log(value);
    }

    fetch(`${BASE_URL}/api/v0.1/` + mode + '/' + value)
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.json();
        })
        .then(data => {
            console.log(data);
        })
        .catch(error => {
            alert('Error applying settings');
            console.error('Error applying settings: ', error);
        });
}

function updateFanSpeedValue() {
    document.getElementById('fanSpeedValue').textContent = document.getElementById('fanSpeedSlider').value;
}

// Show control tab by default
openTab("controlTab");
updateFanSpeedValue();

// If control tab is open refresh status every 5 seconds
if (document.getElementById('controlTab').style.display === 'block') {
    console.log("Control tab is open");
    setInterval(populateStatusSection, 5000);
}
