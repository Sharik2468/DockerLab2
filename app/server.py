from flask import Flask, request, jsonify
from heat_solver import initialize_solver, set_boundary, run_simulation, get_temperature

app = Flask(__name__)

@app.route("/initialize", methods=["POST"])
def initialize():
    data = request.json
    initialize_solver(data["grid_size"], data["time_step"], data["diffusivity"])
    return jsonify({"status": "initialized"})

@app.route("/set_boundary", methods=["POST"])
def set_boundary_condition():
    data = request.json
    set_boundary(data["face"], data["temperature"])
    return jsonify({"status": "boundary set"})

@app.route("/run", methods=["POST"])
def run():
    data = request.json
    run_simulation(data["iterations"])
    return jsonify({"status": "simulation run"})


@app.route("/temperature", methods=["GET"])
def temperature():
    data = request.json
    temp_data = get_temperature(data["grid_size"]).tolist()
    return jsonify({"temperature": temp_data})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
