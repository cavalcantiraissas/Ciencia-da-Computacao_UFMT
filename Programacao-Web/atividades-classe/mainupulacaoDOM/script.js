const MONTANHAS = [
    {nome: "Kilimanjaro", altura: 5895, local: "Tanzania"},
    {nome: "Everest", altura: 8848, local: "Nepal"},
    {nome: "Monte Fuji", altura: 3776, local: "Japan"},
    {nome: "Vaalserberg", altura: 323, local: "Netherlands"},
    {nome: "Denali", altura: 6168, local: "USA"},
    {nome: "Popocatepetl", altura: 5465, local: "Mexico"},
    {nome: "Mont Blanc", altura: 4808, local: "Italy/France"}
];

function criarTabela(montanhas) {
    const container = document.getElementById("mountains");
    container.innerHTML = "";

    const table = document.createElement("table");
    const thead = document.createElement("thead");
    const tbody = document.createElement("tbody");

    const cabecalhos = Object.keys(montanhas[0]);
    const trHead = document.createElement("tr");
    cabecalhos.forEach(chave => {
        const th = document.createElement("th");
        th.textContent = chave.charAt(0).toUpperCase() + chave.slice(1);
        trHead.appendChild(th);
    });
    thead.appendChild(trHead);
    table.appendChild(thead);

    montanhas.forEach(m => {
        const tr = document.createElement("tr");
        cabecalhos.forEach(chave => {
            const td = document.createElement("td");
            td.textContent = m[chave];
            if (typeof m[chave] === "number") {
                td.classList.add("numero");
            }
            tr.appendChild(td);
        });
        tbody.appendChild(tr);
    });

    table.appendChild(tbody);
    container.appendChild(table);
}

document.getElementById("mountainForm").addEventListener("submit", function(event) {
    event.preventDefault();

    const nome = document.getElementById("nome").value.trim();
    const altura = parseFloat(document.getElementById("altura").value);
    const local = document.getElementById("local").value.trim();
    const errorDiv = document.getElementById("errorMessage");

    if (isNaN(altura)) {
        errorDiv.textContent = "A altura deve ser um número válido.";
        return;
    }

    errorDiv.textContent = "";

    MONTANHAS.push({nome, altura, local});
    criarTabela(MONTANHAS);

    document.getElementById("mountainForm").reset();
});

criarTabela(MONTANHAS);
