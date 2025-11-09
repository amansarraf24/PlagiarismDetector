from flask import Flask, render_template, request, jsonify
import os
import subprocess
import tempfile
import shutil
import zipfile

app = Flask(__name__)

# Config paths
BASE_DIR = os.path.abspath(os.path.dirname(__file__))
UPLOAD_DIR = os.path.join(BASE_DIR, "uploads")
EXE_PATH = os.path.abspath(os.path.join(BASE_DIR, "../plagiarism_detector.exe"))

os.makedirs(UPLOAD_DIR, exist_ok=True)


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/analyze", methods=["POST"])
def analyze():
    """Handles .c, .zip, and folder uploads."""
    if "file" not in request.files:
        return jsonify({"error": "No files uploaded"}), 400

    uploaded_files = request.files.getlist("file")
    if not uploaded_files:
        return jsonify({"error": "No files selected"}), 400

    temp_dir = tempfile.mkdtemp(dir=UPLOAD_DIR)

    try:
        # Save uploaded files
        for uploaded in uploaded_files:
            filename = uploaded.filename.replace("\\", "/")
            ext = os.path.splitext(filename)[1].lower()

            if ext not in [".c", ".zip"]:
                continue

            file_path = os.path.join(temp_dir, os.path.basename(filename))
            uploaded.save(file_path)

            if ext == ".zip":
                with zipfile.ZipFile(file_path, "r") as zip_ref:
                    zip_ref.extractall(temp_dir)

        if not any(f.endswith(".c") for f in os.listdir(temp_dir)):
            raise Exception("No .c files found in upload or zip.")

        result = subprocess.run(
            [EXE_PATH, temp_dir],
            capture_output=True,
            text=True,
            timeout=60
        )

        output = result.stdout
        if not output.strip():
            raise Exception("No output from plagiarism_detector.exe")

        summary = parse_output_to_json(output)
        return jsonify(summary)

    except Exception as e:
        return jsonify({"error": str(e)}), 500
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)


def parse_output_to_json(text):
    """Parses plagiarism_detector output to clean JSON."""
    data = {"comparisons": []}
    current = {}

    for line in text.splitlines():
        line = line.strip()
        if not line:
            continue
        if "Comparing:" in line:
            current = {"files": [], "metrics": {}}
        elif line.startswith("File 1:"):
            current["files"].append(line.split(":", 1)[1].strip())
        elif line.startswith("File 2:"):
            current["files"].append(line.split(":", 1)[1].strip())
        elif "OVERALL SCORE" in line:
            current["metrics"]["Overall"] = float(line.split(":")[1].strip().replace("%", ""))
        elif "VERDICT" in line:
            current["metrics"]["Verdict"] = line.split(":", 1)[1].strip()
            data["comparisons"].append(current)
    return data


if __name__ == "__main__":
    print("🚀 Running Plagiarism Detector UI at http://127.0.0.1:5000")
    app.run(debug=True)
