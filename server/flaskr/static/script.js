var socket = io.connect("http://" + document.domain + ":" + location.port);
var chart;

function createChart(data) {
    const ctx = document.getElementById("dataChart").getContext("2d");
    chart = new Chart(ctx, {
        type: "line",
        data: {
            labels: data.map((entry) => entry.Timestamp),
            datasets: [
                {
                    label: "Temperature (°C)",
                    backgroundColor: "rgb(255, 99, 132)",
                    borderColor: "rgb(255, 99, 132)",
                    data: data.map((entry) => entry["Temperature (°C)"]),
                    fill: false,
                },
                {
                    label: "Humidity (%)",
                    backgroundColor: "rgb(54, 162, 235)",
                    borderColor: "rgb(54, 162, 235)",
                    data: data.map((entry) => entry["Humidity (%)"]),
                    fill: false,
                },
            ],
        },
        options: {
            responsive: true,
            maintainAspectRatio: true,
        },
    });
}

function updateChart(data) {
    if (!chart) {
        createChart(data);
    } else {
        chart.data.labels = data.map((entry) => entry.Timestamp);
        chart.data.datasets[0].data = data.map((entry) => entry["Temperature (°C)"]);
        chart.data.datasets[1].data = data.map((entry) => entry["Humidity (%)"]);
        chart.update();
    }
}

function updateTable(data) {
    var tableBody = document.getElementById("dataRows");
    tableBody.innerHTML = ""; // Clear existing rows

    data.forEach(function (entry) {
        var row = tableBody.insertRow();
        var cellTimestamp = row.insertCell(0);
        var cellTemperature = row.insertCell(1);
        var cellHumidity = row.insertCell(2);

        cellTimestamp.innerHTML = entry.Timestamp;
        cellTemperature.innerHTML = entry["Temperature (°C)"];
        cellHumidity.innerHTML = entry["Humidity (%)"];
    });
}

socket.on("connect", function () {
    socket.emit("request data");
});

socket.on("update data", function (msg) {
    updateChart(msg.data);
    // updateTable(msg.data);
});
