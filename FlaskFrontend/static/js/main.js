const analyzeBtn = document.getElementById("analyzeBtn");
const progressBar = document.querySelector(".progress-bar");
const progressContainer = document.getElementById("progress");
const results = document.getElementById("results");

analyzeBtn.addEventListener("click", async () => {
  const cFiles = document.getElementById("cFiles").files;
  const folderFiles = document.getElementById("folderUpload").files;
  const zipFile = document.getElementById("zipUpload").files;

  if (!cFiles.length && !folderFiles.length && !zipFile.length) {
    alert("Please upload at least one file!");
    return;
  }

  const formData = new FormData();
  [...cFiles, ...folderFiles, ...zipFile].forEach(f => formData.append("file", f));

  progressContainer.classList.remove("hidden");
  progressBar.style.width = "0%";
  results.classList.add("hidden");

  const res = await fetch("/analyze", { method: "POST", body: formData });
  const data = await res.json();

  progressBar.style.width = "100%";
  setTimeout(() => progressContainer.classList.add("hidden"), 1000);

  if (data.error) {
    results.innerHTML = `<p style="color:red;">❌ ${data.error}</p>`;
    results.classList.remove("hidden");
    return;
  }

  showResults(data);
});

function showResults(data) {
  results.innerHTML = "<h3>📊 Analysis Results</h3>";
  results.classList.remove("hidden");

  data.comparisons.forEach(cmp => {
    const verdict = cmp.metrics.Verdict;
    const color = getColor(verdict);
    const percent = cmp.metrics.Overall;

    results.innerHTML += `
      <div class="result-card" style="border-color:${color}">
        <div><b>${cmp.files[0]}</b> ↔ <b>${cmp.files[1]}</b></div>
        <div class="bar-container">
          <div style="width:${percent}%;background:${color};height:8px;"></div>
        </div>
        <p style="margin:5px 0 0;">Similarity: <b>${percent}%</b> | Verdict: <span style="color:${color}">${verdict}</span></p>
      </div>`;
  });
}

function getColor(verdict) {
  if (!verdict) return "#fff";
  if (verdict.toLowerCase().includes("high")) return "#e74c3c";
  if (verdict.toLowerCase().includes("medium")) return "#f1c40f";
  return "#2ecc71";
}
