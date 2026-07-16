"""FrotaGesta — ui/tela_dashboard.py"""
import customtkinter as ctk
from db import veiculos as db_v, viagens as db_vi
from ui.componentes import (
    label_titulo, label_secao, card, separador,
    Tabela, btn_primario, btn_perigo, mostrar_sucesso, mostrar_erro, confirmar
)


class TelaDashboard(ctk.CTkFrame):
    def __init__(self, master, **kwargs):
        super().__init__(master, fg_color="transparent", **kwargs)
        self._alerta_sel = None
        self._build()
        self.atualizar()

    def _build(self):
        # Cabeçalho
        topo = ctk.CTkFrame(self, fg_color="transparent")
        topo.pack(fill="x", padx=24, pady=(22, 0))
        label_titulo(topo, "Dashboard").pack(side="left")
        btn_primario(topo, "Atualizar", self.atualizar, largura=110).pack(side="right")
        separador(self).pack(fill="x", padx=24, pady=(12, 16))

        # Cards
        frame_cards = ctk.CTkFrame(self, fg_color="transparent")
        frame_cards.pack(fill="x", padx=24, pady=(0, 18))
        frame_cards.columnconfigure((0, 1, 2, 3), weight=1)

        self._n_disp  = self._card(frame_cards, 0, "Disponíveis",        "#065f46", "#d1fae5", "#064e3b")
        self._n_rota  = self._card(frame_cards, 1, "Em Rota",            "#1e40af", "#dbeafe", "#1e3a8a")
        self._n_manut = self._card(frame_cards, 2, "Manutenção Urgente", "#991b1b", "#fee2e2", "#7f1d1d")
        self._n_alert = self._card(frame_cards, 3, "Alertas Pendentes",  "#92400e", "#fef3c7", "#78350f")

        # Tabela de veículos
        s1 = ctk.CTkFrame(self, fg_color="transparent")
        s1.pack(fill="both", expand=True, padx=24, pady=(0, 8))
        label_secao(s1, "Status da Frota").pack(anchor="w", pady=(0, 6))
        self.tab_veiculos = Tabela(s1, colunas=[
            ("placa",                 "Placa",       100),
            ("modelo",                "Modelo",      220),
            ("tipo",                  "Tipo",         90),
            ("km_atual",              "KM Atual",    100),
            ("status",                "Status",      160),
            ("alertas_pendentes",     "Alertas",      70),
            ("manutencoes_pendentes", "Manutenções", 260),
        ], altura=200)
        self.tab_veiculos.pack(fill="both", expand=True)

        # Tabela de alertas
        s2 = ctk.CTkFrame(self, fg_color="transparent")
        s2.pack(fill="both", expand=True, padx=24, pady=(0, 18))

        cab = ctk.CTkFrame(s2, fg_color="transparent")
        cab.pack(fill="x", pady=(0, 6))
        label_secao(cab, "Alertas de Manutenção Pendentes").pack(side="left")
        btn_perigo(cab, "Marcar como Resolvido",
                   self._resolver, largura=180, altura=30).pack(side="right")

        self.tab_alertas = Tabela(s2, colunas=[
            ("placa",           "Placa",         100),
            ("modelo",          "Modelo",         200),
            ("tipo_manutencao", "Manutenção",     180),
            ("km_alerta",       "KM do Alerta",   110),
            ("dt_alerta",       "Data",           140),
            ("status",          "Status",          90),
        ], altura=150)
        self.tab_alertas.pack(fill="both", expand=True)

    def _card(self, master, col, titulo, cor_txt, cor_bg_l, cor_bg_d):
        f = ctk.CTkFrame(master, corner_radius=8,
                         fg_color=(cor_bg_l, cor_bg_d))
        f.grid(row=0, column=col, padx=(0, 10) if col < 3 else 0, sticky="ew")
        ctk.CTkLabel(f, text="—",
                     font=ctk.CTkFont(size=28, weight="bold"),
                     text_color=(cor_txt, "white")).pack(pady=(14, 2))
        ctk.CTkLabel(f, text=titulo,
                     font=ctk.CTkFont(size=10),
                     text_color=(cor_txt, "white")).pack(pady=(0, 14))
        return f.winfo_children()[0]  # referência ao label do número

    def atualizar(self):
        dados = db_v.painel()
        for d in dados:
            if not d.get("manutencoes_pendentes"):
                d["manutencoes_pendentes"] = "—"
        self.tab_veiculos.popular(dados)

        disp  = sum(1 for d in dados if d["status"] == "Disponível")
        rota  = sum(1 for d in dados if d["status"] == "Em rota")
        manut = sum(1 for d in dados if d["status"] == "Manutenção Urgente")

        self._n_disp.configure(text=str(disp))
        self._n_rota.configure(text=str(rota))
        self._n_manut.configure(text=str(manut))

        alertas = db_vi.alertas_pendentes()
        for a in alertas:
            if a.get("dt_alerta"):
                a["dt_alerta"] = a["dt_alerta"].strftime("%d/%m/%Y %H:%M")
        self.tab_alertas.popular(alertas, on_click=self._sel_alerta)
        self._n_alert.configure(text=str(len(alertas)))
        self._alerta_sel = None

    def _sel_alerta(self, d):
        self._alerta_sel = d

    def _resolver(self):
        if not self._alerta_sel:
            mostrar_erro("Seleção", "Clique em um alerta para selecioná-lo.")
            return
        a = self._alerta_sel
        if confirmar("Resolver", f"Marcar '{a['tipo_manutencao']}' do veículo {a['placa']} como Resolvido?"):
            db_vi.resolver_alerta(a["id"])
            mostrar_sucesso("Resolvido", "Alerta marcado como resolvido.")
            self.atualizar()