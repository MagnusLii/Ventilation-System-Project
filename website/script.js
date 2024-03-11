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
}

function populateHistoryTable() {
    var historyTableBody = document.querySelector('#historyTable tbody');
    
    // Fetch data from the /data/history endpoint - TODO: Change to actual endpoint
    fetch('/api/v0.1/readings/all')
        .then(response => response.json())
        .then(data => {
            data.forEach(function (entry) {
                var row = document.createElement('tr');
                row.innerHTML = '<td>' + entry.index + '</td>' +
                                '<td>' + entry.date + '</td>' +
                                '<td>' + entry.fanSpeed + '</td>' +
                                '<td>' + entry.setPoint + '</td>' +
                                '<td>' + entry.pressure + '</td>' +
                                '<td>' + entry.co2 + '</td>' +
                                '<td>' + entry.humidity + '</td>' +
                                '<td>' + entry.temperature + '</td>';
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
    var logTableBody = document.querySelector('#logTable tbody');

    // Fetch data from the /data/logs endpoint - TODO: Change to actual endpoint
    fetch('http://127.0.0.1:5000/api/v0.1/logs/all')
        .then(response => response.json())
        .then(data => {
            data.forEach(function (entry) {
                var row = document.createElement('tr');
                row.innerHTML = '<td>' + entry.index + '</td>' +
                                '<td>' + entry.logcode + '</td>' +
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
    // Fetch data from the /data/status - TODO: Change to actual endpoint
    fetch('/api/v0.1/status')
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.json();
        })
        .then(data => {
            document.getElementById('fanSpeedStatus').textContent = data.fanSpeed;
            document.getElementById('setPointStatus').textContent = data.setPoint;
            document.getElementById('pressureStatus').textContent = data.pressure;
            document.getElementById('co2Status').textContent = data.co2;
            document.getElementById('humidityStatus').textContent = data.humidity;
            document.getElementById('temperatureStatus').textContent = data.temperature;
        })
        .catch(error => {
            console.error('Error fetching status data: ', error);
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
        console.log(value);
    } else if (mode === 0) {
        value = document.getElementById('fanSpeedSlider').value;
        console.log(value);
    }

    fetch('/api/v0.1/' + mode + '/' + value)
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

populateHistoryTable();
populateLogTable();
populateStatusSection();

// Show control tab by default
openTab("controlTab");

