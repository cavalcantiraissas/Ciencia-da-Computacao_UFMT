const searchInput = document.getElementById('search-input');
const searchButton = document.getElementById('search-button');
const resultsContainer = document.getElementById('results');

searchButton.addEventListener('click', buscarDados);
searchInput.addEventListener('keypress', function(e) {
    if (e.key === 'Enter') {
        buscarDados();
    }
});

async function buscarDados() {
    const pais = searchInput.value.trim();
    
    if (!pais) {
        exibirErro('Por favor, digite o nome de um país.');
        return;
    }
    
    resultsContainer.innerHTML = '<div class="loading">Buscando universidades...</div>';
    
    try {
        const url = `http://universities.hipolabs.com/search?country=${encodeURIComponent(pais)}`;
        
        const response = await fetch(url);
        
        if (!response.ok) {
            throw new Error(`Erro na requisição: ${response.status}`);
        }
        
        const dados = await response.json();
        
        exibirResultados(dados);
    } catch (erro) {
        exibirErro(`Falha ao buscar dados: ${erro.message}`);
    }
}

function exibirResultados(dados) {
    resultsContainer.innerHTML = '';
    
    if (dados.length === 0) {
        resultsContainer.innerHTML = '<div class="no-results">Nenhuma universidade encontrada para o país especificado.</div>';
        return;
    }
    
    dados.forEach(universidade => {
        const card = document.createElement('div');
        card.className = 'university-card';
        
        let websiteHtml = '';
        if (universidade.domains && universidade.domains.length > 0) {
            const domain = universidade.domains[0];
            websiteHtml = `
                <a href="http://${domain}" target="_blank" class="website-link">
                    Visitar site da universidade
                </a>
            `;
        }
        
        card.innerHTML = `
            <h3 class="university-name">${universidade.name}</h3>
            <div class="university-detail">
                <i>📍</i> <span>${universidade.country}</span>
            </div>
            ${universidade['state-province'] ? `
            <div class="university-detail">
                <i>🏙️</i> <span>${universidade['state-province']}</span>
            </div>
            ` : ''}
            ${websiteHtml}
        `;
        
        resultsContainer.appendChild(card);
    });
}

function exibirErro(mensagem) {
    resultsContainer.innerHTML = `<div class="error">${mensagem}</div>`;
}
