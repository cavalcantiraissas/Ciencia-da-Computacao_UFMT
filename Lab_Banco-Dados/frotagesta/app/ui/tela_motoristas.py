"""FrotaGesta — ui/tela_motoristas.py"""
import customtkinter as ctk
from db import motoristas as db_m
from ui.validacoes import validar_cpf, formatar_cpf
from ui.componentes import (
    label_titulo, label_secao, label_campo, card, separador,
    Tabela, btn_primario, btn_sucesso, btn_perigo, btn_outline,
    campo, dropdown, mostrar_erro, mostrar_sucesso, confirmar
)

CATEGORIAS = ["A", "B", "C", "D", "E", "AB", "AC", "AD", "AE"]


class TelaMotoristas(ctk.CTkFrame):
    def __init__(self, master, **kwargs):
        super().__init__(master, fg_color="transparent", **kwargs)
        self._sel = None
        self._build()
        self.atualizar()

    def _build(self):
        topo = ctk.CTkFrame(self, fg_color="transparent")
        topo.pack(fill="x", padx=24, pady=(22, 0))
        label_titulo(topo, "Motoristas").pack(side="left")
        btn_primario(topo, "Atualizar", self.atualizar, largura=110).pack(side="right")
        separador(self).pack(fill="x", padx=24, pady=(12, 16))

        body = ctk.CTkFrame(self, fg_color="transparent")
        body.pack(fill="both", expand=True, padx=24, pady=(0, 20))
        body.columnconfigure(0, weight=3)
        body.columnconfigure(1, weight=0)
        body.rowconfigure(0, weight=1)

        # Tabela
        esq = ctk.CTkFrame(body, fg_color="transparent")
        esq.grid(row=0, column=0, sticky="nsew", padx=(0, 16))
        label_secao(esq, "Motoristas cadastrados").pack(anchor="w", pady=(0, 6))
        self.tabela = Tabela(esq, colunas=[
            ("cpf",           "CPF",   145),
            ("nome",          "Nome",  220),
            ("categoria_cnh", "CNH",    60),
            ("ativo",         "Ativo",  60),
        ], altura=500)
        self.tabela.pack(fill="both", expand=True)

        # Formulário
        form = card(body, width=300)
        form.grid(row=0, column=1, sticky="nsew")
        form.grid_propagate(False)

        self._titulo_form = ctk.CTkLabel(form, text="Novo Motorista",
                                          font=ctk.CTkFont(size=13, weight="bold"),
                                          anchor="w")
        self._titulo_form.pack(anchor="w", padx=16, pady=(16, 2))
        separador(form).pack(fill="x", padx=16, pady=(8, 14))

        for label_txt, attr, ph in [
            ("CPF",            "f_cpf",  "000.000.000-00"),
            ("Nome completo",  "f_nome", "Nome do motorista"),
        ]:
            label_campo(form, label_txt).pack(anchor="w", padx=16)
            w = campo(form, ph, largura=268)
            w.pack(padx=16, pady=(3, 10))
            setattr(self, attr, w)

        label_campo(form, "Categoria CNH").pack(anchor="w", padx=16)
        self.f_cnh, self.v_cnh = dropdown(form, CATEGORIAS, largura=268)
        self.f_cnh.pack(padx=16, pady=(3, 10))

        self.v_ativo = ctk.BooleanVar(value=True)
        ctk.CTkCheckBox(form, text="Motorista ativo",
                         variable=self.v_ativo,
                         font=ctk.CTkFont(size=12)).pack(anchor="w", padx=16, pady=(0, 14))

        separador(form).pack(fill="x", padx=16, pady=(0, 14))

        btns = ctk.CTkFrame(form, fg_color="transparent")
        btns.pack(fill="x", padx=16, pady=(0, 10))
        btn_sucesso(btns, "Salvar", self._salvar, largura=110).pack(side="left")
        btn_outline(btns, "Limpar", self._limpar, largura=90).pack(side="left", padx=(8, 0))

        separador(form).pack(fill="x", padx=16, pady=(10, 14))
        btn_perigo(form, "Excluir Selecionado", self._excluir, largura=268).pack(padx=16, pady=(0, 16))

        self.tabela.popular([], on_click=self._carregar)

    def atualizar(self):
        dados = db_m.listar()
        for d in dados:
            d["ativo"] = "Sim" if d["ativo"] else "Não"
        self.tabela.popular(dados, on_click=self._carregar)
        self._limpar()

    def _carregar(self, d):
        self._sel = d
        self._titulo_form.configure(text="Editar Motorista")
        self.f_cpf.delete(0, "end");  self.f_cpf.insert(0, d.get("cpf", ""))
        self.f_nome.delete(0, "end"); self.f_nome.insert(0, d.get("nome", ""))
        self.v_cnh.set(d.get("categoria_cnh", CATEGORIAS[0]))
        self.v_ativo.set(d.get("ativo", "Sim") == "Sim")

    def _limpar(self):
        self._sel = None
        self._titulo_form.configure(text="Novo Motorista")
        self.f_cpf.delete(0, "end")
        self.f_nome.delete(0, "end")
        self.v_cnh.set(CATEGORIAS[0])
        self.v_ativo.set(True)

    def _salvar(self):
        cpf  = self.f_cpf.get().strip()
        nome = self.f_nome.get().strip()
        cnh  = self.v_cnh.get()
        ativo = self.v_ativo.get()

        if not validar_cpf(cpf):
            mostrar_erro("Validação", "CPF inválido.\nVerifique o número e use o formato 000.000.000-00."); return
        cpf = formatar_cpf(cpf)  # garante formatação padronizada
        if not nome or len(nome) < 3:
            mostrar_erro("Validação", "Nome deve ter ao menos 3 caracteres."); return

        try:
            if self._sel:
                db_m.atualizar(self._sel["id"], cpf, nome, cnh, ativo)
                mostrar_sucesso("Sucesso", "Motorista atualizado.")
            else:
                db_m.cadastrar(cpf, nome, cnh)
                mostrar_sucesso("Sucesso", "Motorista cadastrado.")
            self.atualizar()
        except Exception as e:
            msg = str(e)
            if "unique" in msg.lower():
                mostrar_erro("Erro", "Já existe um motorista com este CPF.")
            else:
                mostrar_erro("Erro", msg)

    def _excluir(self):
        if not self._sel:
            mostrar_erro("Seleção", "Clique em um motorista para selecioná-lo."); return
        nome = self._sel.get("nome", "")
        if not confirmar("Excluir", f"Excluir o motorista '{nome}'?\n\nEsta ação não pode ser desfeita."):
            return
        try:
            db_m.excluir(self._sel["id"])
            mostrar_sucesso("Excluído", "Motorista removido com sucesso.")
            self.atualizar()
        except Exception as e:
            msg = str(e)
            if "foreign key" in msg.lower() or "fk" in msg.lower() or "viagens" in msg.lower():
                mostrar_erro("Erro", "Não é possível excluir um motorista com viagens registradas.")
            else:
                mostrar_erro("Erro", msg)