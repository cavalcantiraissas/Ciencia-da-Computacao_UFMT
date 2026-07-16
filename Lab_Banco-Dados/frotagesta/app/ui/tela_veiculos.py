"""FrotaGesta — ui/tela_veiculos.py"""
import customtkinter as ctk
from datetime import datetime
from db import veiculos as db_v
from ui.validacoes import validar_placa
from ui.componentes import (
    label_titulo, label_secao, label_campo, card, separador,
    Tabela, btn_primario, btn_sucesso, btn_perigo, btn_outline,
    campo, dropdown, mostrar_erro, mostrar_sucesso, confirmar
)

TIPOS  = ["Carro", "Van", "Caminhão"]
STATUS = ["Disponível", "Em rota", "Manutenção Urgente", "Em manutenção"]
ANO_MIN, ANO_MAX = 1950, datetime.now().year + 1


class TelaVeiculos(ctk.CTkFrame):
    def __init__(self, master, **kwargs):
        super().__init__(master, fg_color="transparent", **kwargs)
        self._sel = None
        self._build()
        self.atualizar()

    def _build(self):
        topo = ctk.CTkFrame(self, fg_color="transparent")
        topo.pack(fill="x", padx=24, pady=(22, 0))
        label_titulo(topo, "Veículos").pack(side="left")
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
        label_secao(esq, "Veículos cadastrados").pack(anchor="w", pady=(0, 6))
        self.tabela = Tabela(esq, colunas=[
            ("placa",    "Placa",    100),
            ("modelo",   "Modelo",   200),
            ("ano",      "Ano",       60),
            ("tipo",     "Tipo",      90),
            ("km_atual", "KM Atual", 100),
            ("status",   "Status",   155),
        ], altura=500)
        self.tabela.pack(fill="both", expand=True)

        # Formulário
        form = card(body, width=300)
        form.grid(row=0, column=1, sticky="nsew")
        form.grid_propagate(False)

        self._titulo_form = ctk.CTkLabel(form, text="Novo Veículo",
                                          font=ctk.CTkFont(size=13, weight="bold"),
                                          anchor="w")
        self._titulo_form.pack(anchor="w", padx=16, pady=(16, 2))
        separador(form).pack(fill="x", padx=16, pady=(8, 14))

        for label_txt, attr, ph in [
            ("Placa",              "f_placa",  "ABC-1234 ou ABC1D23"),
            ("Modelo",             "f_modelo", "Ex.: VW Delivery 11.180"),
            ("Ano de fabricação",  "f_ano",    str(datetime.now().year)),
            ("KM atual",           "f_km",     "Ex.: 48500"),
        ]:
            label_campo(form, label_txt).pack(anchor="w", padx=16)
            w = campo(form, ph, largura=268)
            w.pack(padx=16, pady=(3, 10))
            setattr(self, attr, w)

        label_campo(form, "Tipo de veículo").pack(anchor="w", padx=16)
        self.f_tipo, self.v_tipo = dropdown(form, TIPOS, largura=268)
        self.f_tipo.pack(padx=16, pady=(3, 10))

        # Status (só aparece na edição)
        self._frame_status = ctk.CTkFrame(form, fg_color="transparent")
        self._frame_status.pack(fill="x", padx=16)

        separador(form).pack(fill="x", padx=16, pady=(10, 14))
        btns = ctk.CTkFrame(form, fg_color="transparent")
        btns.pack(fill="x", padx=16, pady=(0, 10))
        btn_sucesso(btns, "Salvar", self._salvar, largura=110).pack(side="left")
        btn_outline(btns, "Limpar", self._limpar, largura=90).pack(side="left", padx=(8, 0))

        separador(form).pack(fill="x", padx=16, pady=(10, 14))
        btn_perigo(form, "Excluir Selecionado", self._excluir, largura=268).pack(padx=16, pady=(0, 16))

        self.tabela.popular([], on_click=self._carregar)

    def atualizar(self):
        self.tabela.popular(db_v.listar(), on_click=self._carregar)
        self._limpar()

    def _carregar(self, d):
        self._sel = d
        self._titulo_form.configure(text="Editar Veículo")
        self.f_placa.delete(0, "end");  self.f_placa.insert(0, d.get("placa", ""))
        self.f_modelo.delete(0, "end"); self.f_modelo.insert(0, d.get("modelo", ""))
        self.f_ano.delete(0, "end");    self.f_ano.insert(0, str(d.get("ano", "")))
        self.f_km.delete(0, "end");     self.f_km.insert(0, str(d.get("km_atual", "")))
        self.v_tipo.set(d.get("tipo", TIPOS[0]))

        for w in self._frame_status.winfo_children():
            w.destroy()
        label_campo(self._frame_status, "Status").pack(anchor="w")
        self.f_status, self.v_status = dropdown(self._frame_status, STATUS, largura=268)
        self.v_status.set(d.get("status", STATUS[0]))
        self.f_status.pack(pady=(3, 0))

    def _limpar(self):
        self._sel = None
        self._titulo_form.configure(text="Novo Veículo")
        for attr in ("f_placa", "f_modelo", "f_ano", "f_km"):
            getattr(self, attr).delete(0, "end")
        self.v_tipo.set(TIPOS[0])
        for w in self._frame_status.winfo_children():
            w.destroy()

    def _salvar(self):
        placa  = self.f_placa.get().strip().upper()
        modelo = self.f_modelo.get().strip()
        ano_s  = self.f_ano.get().strip()
        km_s   = self.f_km.get().strip()
        tipo   = self.v_tipo.get()

        if not validar_placa(placa):
            mostrar_erro("Validação", "Placa inválida.\nUse o formato ABC-1234 (antigo) ou ABC1D23 (Mercosul)."); return
        if not modelo:
            mostrar_erro("Validação", "Modelo é obrigatório."); return
        if not ano_s.isdigit() or not (ANO_MIN <= int(ano_s) <= ANO_MAX):
            mostrar_erro("Validação", f"Ano deve ser entre {ANO_MIN} e {ANO_MAX}."); return
        try:
            km = float(km_s.replace(",", "."))
            assert km >= 0
        except:
            mostrar_erro("Validação", "KM deve ser um número positivo."); return

        try:
            if self._sel:
                status = self.v_status.get() if hasattr(self, "v_status") else self._sel["status"]
                db_v.atualizar(self._sel["id"], placa, modelo, int(ano_s), km, tipo, status)
                mostrar_sucesso("Sucesso", "Veículo atualizado.")
            else:
                db_v.cadastrar(placa, modelo, int(ano_s), km, tipo)
                mostrar_sucesso("Sucesso", "Veículo cadastrado.")
            self.atualizar()
        except Exception as e:
            msg = str(e)
            if "unique" in msg.lower():
                mostrar_erro("Erro", "Já existe um veículo com esta placa.")
            else:
                mostrar_erro("Erro", msg)

    def _excluir(self):
        if not self._sel:
            mostrar_erro("Seleção", "Clique em um veículo para selecioná-lo."); return
        placa = self._sel.get("placa", "")
        if not confirmar("Excluir", f"Excluir o veículo '{placa}'?\n\nEsta ação não pode ser desfeita."):
            return
        try:
            db_v.excluir(self._sel["id"])
            mostrar_sucesso("Excluído", "Veículo removido com sucesso.")
            self.atualizar()
        except Exception as e:
            msg = str(e)
            if "foreign key" in msg.lower() or "viagens" in msg.lower():
                mostrar_erro("Erro", "Não é possível excluir um veículo com viagens registradas.")
            else:
                mostrar_erro("Erro", msg)