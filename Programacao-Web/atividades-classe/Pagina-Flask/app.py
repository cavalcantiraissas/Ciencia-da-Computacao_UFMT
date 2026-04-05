from flask import Flask, render_template, request, redirect, url_for
import sqlite3
import os

app = Flask(__name__)

# Configurações do banco de dados
DATABASE = 'produtos.db'

def init_db():
    """Inicializa o banco de dados e cria a tabela se não existir"""
    with sqlite3.connect(DATABASE) as conn:
        cursor = conn.cursor()
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS produtos (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                nome TEXT NOT NULL,
                marca TEXT NOT NULL,
                valor REAL NOT NULL
            )
        ''')
        conn.commit()

@app.route('/')
def index():
    """Página inicial com apresentação simples"""
    return render_template('index.html')

@app.route('/cadastrar', methods=['GET', 'POST'])
def cadastrar():
    """Página para cadastrar novos produtos"""
    if request.method == 'POST':
        # Obter dados do formulário
        nome = request.form['nome']
        marca = request.form['marca']
        valor = request.form['valor']
        
        # Inserir no banco de dados
        with sqlite3.connect(DATABASE) as conn:
            cursor = conn.cursor()
            cursor.execute('INSERT INTO produtos (nome, marca, valor) VALUES (?, ?, ?)', 
                          (nome, marca, valor))
            conn.commit()
        
        return redirect(url_for('listar'))
    
    return render_template('cadastrar.html')

@app.route('/listar')
def listar():
    """Página para listar todos os produtos cadastrados"""
    with sqlite3.connect(DATABASE) as conn:
        cursor = conn.cursor()
        cursor.execute('SELECT * FROM produtos')
        produtos = cursor.fetchall()
    
    return render_template('listar.html', produtos=produtos)

if __name__ == '__main__':
    init_db()  # Garante que o banco existe
    app.run(debug=True)
