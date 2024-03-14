import datetime
from secrets import token_urlsafe

from flask import Flask, jsonify, render_template, request
from flask_socketio import SocketIO, emit

app = Flask(__name__)
app.config["SECRET_KEY"] = token_urlsafe(32)

socketio = SocketIO(app)


def read_data():
    data = []
    with open("flaskr/data.txt", "r") as file:
        lines = file.readlines()
        for line in lines[1:]:  # Skip the header line
            values = line.strip().split()
            data.append(
                {
                    "Timestamp": values[2],
                    "Temperature (°C)": float(values[0]),
                    "Humidity (%)": int(float(values[1])),
                }
            )

    return data


@app.get("/")
def index():
    data = read_data()
    return render_template("index.html", data=data)


@app.route("/api/dht", methods=["POST"])
def add_data():
    try:
        new_data = request.get_json()
        if new_data:
            timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            temperature = new_data["temperature"]
            humidity = new_data["humidity"]

            with open("flaskr/data.txt", "a") as file:
                file.write(f"\n{temperature} {humidity} {timestamp}")

            data = read_data()
            socketio.emit("update data", {"data": data})

            return jsonify({"message": "Data added successfully"}), 201
        else:
            return jsonify({"error": "Invalid JSON data"}), 400
    except Exception as e:
        print(e)
        return jsonify({"error": str(e)}), 500


@socketio.on("connect")
def test_connect():
    emit("after connect", {"data": "Connected"})


@socketio.on("request data")
def handle_request_data():
    data = read_data()
    emit("update data", {"data": data})


if __name__ == "__main__":
    socketio.run(app)
