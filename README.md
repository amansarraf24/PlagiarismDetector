# 🧠 Code Plagiarism Detector  
🚀 An Intelligent Structural Plagiarism Detection System for C Programs  

<br>

## 📄 Overview  
Code Plagiarism Detector ek complete end-to-end system hai jo **C programming files** ke beech code similarity detect karta hai using **AST (Abstract Syntax Tree)**, **CFG (Control Flow Graph)**, and **DAG (Dependency Graph)** analysis.  

Ye project **C language**, **Flask (Python)**, aur **modern web frontend (HTML, CSS, JS)** ka hybrid integration hai —  
jisme backend static analysis karta hai aur frontend visually professional dashboard me result show karta hai.  

<br>

## 🎯 Features  
✅ Upload multiple `.c` files, folders, or ZIP archives  
✅ Detect code plagiarism structurally (not just text comparison)  
✅ Uses AST + CFG + DAG for logical similarity  
✅ Shows percentage-based similarity with color indicators:  
🔴 High (80–100%) → High Plagiarism  
🟡 Medium (50–79%) → Moderate Similarity  
🟢 Low (0–49%) → Unique / Original  
✅ Displays detailed analysis of AST, CFG, DAG scores and final verdict  
✅ Beautiful, glass-morphic frontend UI  
✅ Interactive and fully dynamic — no page reloads  
✅ Cross-platform support (Windows, Linux, macOS)  

<br>

## ⚙️ Tech Stack  

| Layer | Technology Used | Purpose |
|--------|------------------|----------|
| Frontend | HTML, CSS, JavaScript | UI for file upload & visualization |
| Backend | Flask (Python) | Handles upload, executes analyzer, returns JSON |
| Core Engine | C Language | Performs actual AST, CFG, DAG comparison |
| Visualization | Vanilla JS + Dynamic Cards | Displays colored results and details |
| Storage | Local file processing | Temporary file analysis without DB |

<br>

## 🧩 System Architecture  
User → Frontend (HTML/CSS/JS) → Flask Server (Python) → Plagiarism Detector Engine (C) → AST / CFG / DAG Analysis → Result JSON → Rendered as Visual Report  

<br>

## 🧠 Working Process  
1️⃣ **User Uploads Files** — Multiple `.c` files, folder, or `.zip` archive.  
2️⃣ **Flask Backend Saves Files** — Files extracted in a temporary `/uploads` folder.  
3️⃣ **Backend Executes C Engine** — Runs the compiled `plagiarism_detector.exe` program.  
4️⃣ **C Engine Performs Deep Analysis** — Builds AST, CFG, and DAG for each file pair and calculates structural similarity metrics.  
5️⃣ **Flask Converts Results into JSON** — Structured output returned to frontend.  
6️⃣ **Frontend Displays Colored Results** — Each file pair shown in a card with similarity % and detailed breakdown.  

<br>

## 💻 Folder Structure 
```
PlagiarismDetector/
│
├── src/ # Core C backend engine
│ ├── ast.c / ast.h
│ ├── cfg.c / cfg.h
│ ├── dag.c / dag.h
│ ├── parser.c / parser.h
│ ├── normalizer.c / normalizer.h
│ ├── detector.c / detector.h
│ ├── utils.c / utils.h
│ ├── main.c # Entry point for C engine
│ └── plagiarism_detector.exe
│
├── FlaskFrontend/
│ ├── app.py # Flask backend API
│ ├── templates/
│ │ └── index.html # Main web interface
│ ├── static/
│ │ ├── css/style.css
│ │ └── js/main.js
│ └── uploads/ # Temporary upload folder
│
├── test_files/ # Sample test C files
│ ├── loop_for.c
│ ├── loop_while.c
│ ├── sample1.c
│ ├── sample2.c
│ └── testfiles5.c
│
└── README.md
```

<br>

## 🧪 How to Run Locally  

🖥 **Backend Compilation (C Engine)**  
```
cd PlagiarismDetector/src  
gcc -std=c99 -Wall -O2 -o plagiarism_detector.exe main.c directory_handler.c file_handler.c utils.c lexer.c ast.c parser.c normalizer.c cfg.c dag.c detector.c
🐍 Flask Setup


Copy code
cd ../FlaskFrontend  
pip install flask  
python app.py
🌐 Open in Browser:
👉 http://127.0.0.1:5000


📊 Output Preview
Upload Section

Upload .c files, folders, or .zip

Hit “Start Analysis” button

Results Section
Each comparison shown in clean card format:

yaml
Copy code
loop_for.c ↔ loop_while.c  
Similarity: 85%  
Verdict: High Plagiarism  
AST: 87% | CFG: 90% | DAG: 85%
Colors indicate plagiarism level:
🔴 High → High Similarity
🟡 Medium → Some Similarity
🟢 Low → Minimal / None
```
<br>
## 🧭 Future Enhancements
🚀 Add PDF Report Export feature
📁 Integrate SQL database for submission records
📊 Add code visualization graphs (AST tree view)
🌐 Deploy on Render / Heroku / Vercel
🧬 Extend support for C++, Java, and Python

<br>
## 👨‍💻 Authors
Aman Verma
Aniket Pratap Singh
Shrishti Yadav
Atulya Gupta

🎓 B.Tech in Computer Engineering
💡 Passionate about Software Engineering, Backend Development & Code Intelligence

<br>
⭐ Support
If you found this project helpful, please consider giving it a
🌟 Star on GitHub — it motivates us to build more amazing projects 💻❤️
