"""FrotaGesta — ui/tela_operacao.py"""
import customtkinter as ctk
from db import motoristas as db_m, veiculos as db_v, viagens as db_vi
from ui.componentes import (
    label_titulo, label_secao, label_campo, card, separador,
    Tabela, btn_sucesso, btn_perigo, btn_primario, btn_outline, btn_neutro,
    campo, mostrar_erro, mostrar_sucesso, confirmar
)


class TelaOperacao(ctk.CTkFrame):
    def __init__(self, master, **kwargs):
        super().__init__(master, fg_color="transparent", **kwargs)
        self._viagem_sel = None
        self._build()
        self.atualizar()

    def _build(self):
        topo = ctk.CTkFrame(self, fg_color="transparent")
        topo.pack(fill="x", padx=24, pady=(22, 0))
        label_titulo(topo, "Operação de Viagens").pack(side="left")
        btn_primario(topo, "Atualizar", self.atualizar, largura=110).pack(side="right")
        separador(self).pack(fill="x", padx=24, pady=(12, 16))

        body = ctk.CTkFrame(self, fg_color="transparent")
        body.pack(fill="both", expand=True, padx=24, pady=(0, 20))
        body.columnconfigure(0, weight=3)
        body.columnconfigure(1, weight=2)
        body.rowconfigure(0, weight=1)

        # ── Coluna esquerda ───────────────────────────────────────────────────
        esq = ctk.CTkFrame(body, fg_color="transparent")
        esq.grid(row=0, column=0, sticky="nsew", padx=(0, 16))
        esq.rowconfigure(1, weight=1)

        label_secao(esq, "Viagens em andamento").grid(row=0, column=0, sticky="w", pady=(0, 6))

        self.tab_viagens = Tabela(esq, colunas=[
            ("id",         "ID",        46),
            ("motorista",  "Motorista", 190),
            ("placa",      "Placa",      90),
            ("dt_saida",   "Saída",     130),
            ("km_inicial", "KM Ini",     80),
        ], altura=220)
        self.tab_viagens.grid(row=1, column=0, sticky="nsew")

        # Painel encerrar
        frame_enc = card(esq)
        frame_enc.grid(row=2, column=0, sticky="ew", pady=(14, 0))

        ctk.CTkLabel(frame_enc, text="Encerrar Viagem",
                     font=ctk.CTkFont(size=13, weight="bold"),
                     anchor="w").pack(anchor="w", padx=16, pady=(14, 2))
        separador(frame_enc).pack(fill="x", padx=16, pady=(6, 10))

        self._lbl_sel = ctk.CTkLabel(frame_enc,
                                      text="Nenhuma viagem selecionada",
                                      font=ctk.CTkFont(size=11, slant="italic"),
                                      text_color=("gray50", "gray55"))
        self._lbl_sel.pack(anchor="w", padx=16, pady=(0, 10))

        label_campo(frame_enc, "KM Final").pack(anchor="w", padx=16)
        self.f_km_final = campo(frame_enc, "Ex.: 49200", largura=320)
        self.f_km_final.pack(padx=16, pady=(3, 10))

        label_campo(frame_enc, "Observações (opcional)").pack(anchor="w", padx=16)
        self.f_obs = ctk.CTkTextbox(frame_enc, height=56, width=320,
                                     font=ctk.CTkFont(size=12), corner_radius=6)
        self.f_obs.pack(padx=16, pady=(3, 12))

        btns_enc = ctk.CTkFrame(frame_enc, fg_color="transparent")
        btns_enc.pack(fill="x", padx=16, pady=(0, 14))
        btn_sucesso(btns_enc, "Concluir Viagem", self._concluir, largura=150).pack(side="left")
        btn_perigo(btns_enc,  "Cancelar Viagem", self._cancelar, largura=150).pack(side="left", padx=(10, 0))

        # ── Coluna direita ────────────────────────────────────────────────────
        dir_ = card(body)
        dir_.grid(row=0, column=1, sticky="nsew")

        ctk.CTkLabel(dir_, text="Iniciar Nova Viagem",
                     font=ctk.CTkFont(size=13, weight="bold"),
                     anchor="w").pack(anchor="w", padx=16, pady=(16, 2))
        separador(dir_).pack(fill="x", padx=16, pady=(6, 14))

        label_campo(dir_, "Motorista").pack(anchor="w", padx=16)
        self.v_mot = ctk.StringVar()
        self.cb_mot = ctk.CTkComboBox(dir_, variable=self.v_mot,
                                       width=300, height=34,
                                       font=ctk.CTkFont(size=12),
                                       state="readonly", corner_radius=6)
        self.cb_mot.pack(padx=16, pady=(3, 10))

        label_campo(dir_, "Veículo disponível").pack(anchor="w", padx=16)
        self.v_veic = ctk.StringVar()
        self.cb_veic = ctk.CTkComboBox(dir_, variable=self.v_veic,
                                        width=300, height=34,
                                        font=ctk.CTkFont(size=12),
                                        state="readonly", corner_radius=6)
        self.cb_veic.pack(padx=16, pady=(3, 10))

        label_campo(dir_, "KM Inicial (odômetro)").pack(anchor="w", padx=16)
        self.f_km_ini = campo(dir_, "Ex.: 48500", largura=300)
        self.f_km_ini.pack(padx=16, pady=(3, 14))

        separador(dir_).pack(fill="x", padx=16, pady=(0, 14))
        btn_sucesso(dir_, "Iniciar Viagem", self._iniciar, largura=300, altura=36).pack(padx=16, pady=(0, 16))

        separador(dir_).pack(fill="x", padx=16, pady=(0, 12))
        label_secao(dir_, "Últimas viagens").pack(anchor="w", padx=16, pady=(0, 6))
        self.tab_hist = Tabela(dir_, colunas=[
            ("placa",        "Placa",     90),
            ("motorista",    "Motorista", 160),
            ("km_percorrida","KM",         60),
            ("status",       "Status",    100),
        ], altura=180)
        self.tab_hist.pack(padx=16, pady=(0, 16), fill="both", expand=True)

        self.tab_viagens.popular([], on_click=self._sel_viagem)

    def atualizar(self):
        # Motoristas
        mots = db_m.listar_ativos()
        self._map_mot = {f"{m['nome']} (CNH {m['categoria_cnh']})": m["id"] for m in mots}
        self.cb_mot.configure(values=list(self._map_mot.keys()))
        if self._map_mot:
            self.v_mot.set(list(self._map_mot.keys())[0])

        # Veículos disponíveis
        veics = db_v.listar_disponiveis()
        self._map_veic = {f"{v['placa']} — {v['modelo']} ({v['tipo']})": v for v in veics}
        self.cb_veic.configure(values=list(self._map_veic.keys()))
        if self._map_veic:
            self.v_veic.set(list(self._map_veic.keys())[0])

        # Viagens em andamento
        em_and = db_vi.em_andamento()
        for v in em_and:
            if v.get("dt_saida"):
                v["dt_saida"] = v["dt_saida"].strftime("%d/%m %H:%M")
        self.tab_viagens.popular(em_and, on_click=self._sel_viagem)

        # Histórico
        self.tab_hist.popular(db_vi.historico()[:10])

        self._viagem_sel = None
        self._lbl_sel.configure(text="Nenhuma viagem selecionada")
        self.f_km_final.delete(0, "end")
        self.f_obs.delete("1.0", "end")

    def _sel_viagem(self, d):
        self._viagem_sel = d
        self._lbl_sel.configure(
            text=f"Viagem #{d['id']}  |  {d['motorista']}  |  {d['placa']}  |  KM ini: {d['km_inicial']}")

    def _iniciar(self):
        mot_txt  = self.v_mot.get()
        veic_txt = self.v_veic.get()
        km_s     = self.f_km_ini.get().strip()

        if not mot_txt:
            mostrar_erro("Validação", "Selecione um motorista."); return
        if not veic_txt:
            mostrar_erro("Validação", "Selecione um veículo."); return
        try:
            km = float(km_s.replace(",", "."))
            assert km >= 0
        except:
            mostrar_erro("Validação", "KM inicial deve ser um número positivo."); return

        mot_id = self._map_mot[mot_txt]
        veic   = self._map_veic[veic_txt]

        if not confirmar("Confirmar", f"Iniciar viagem?\n\nMotorista: {mot_txt}\nVeículo: {veic_txt}\nKM inicial: {km:.0f}"):
            return
        try:
            db_vi.iniciar(mot_id, veic["id"], km)
            mostrar_sucesso("Sucesso", "Viagem iniciada.")
            self.atualizar()
        except Exception as e:
            msg = str(e)
            if "não está habilitado" in msg or "cnh" in msg.lower():
                mostrar_erro("CNH Inválida", f"Este motorista não pode conduzir este tipo de veículo.\n\n{msg}")
            else:
                mostrar_erro("Erro", msg)

    def _concluir(self):
        if not self._viagem_sel:
            mostrar_erro("Seleção", "Selecione uma viagem na tabela."); return
        km_s = self.f_km_final.get().strip()
        try:
            km = float(km_s.replace(",", "."))
            assert km >= 0
        except:
            mostrar_erro("Validação", "KM final deve ser um número positivo."); return

        obs = self.f_obs.get("1.0", "end").strip() or None
        v   = self._viagem_sel

        if not confirmar("Confirmar", f"Concluir viagem #{v['id']}?\nKM final: {km:.0f}\n\nEsta ação não pode ser desfeita."):
            return
        try:
            db_vi.encerrar(v["id"], km, obs)
            mostrar_sucesso("Sucesso", "Viagem concluída. Verifique o Dashboard para alertas de manutenção.")
            self.atualizar()
        except Exception as e:
            msg = str(e)
            if "menor" in msg or "km" in msg.lower():
                mostrar_erro("KM Inválida", msg)
            else:
                mostrar_erro("Erro", msg)

    def _cancelar(self):
        if not self._viagem_sel:
            mostrar_erro("Seleção", "Selecione uma viagem na tabela."); return
        v = self._viagem_sel
        if not confirmar("Cancelar Viagem",
                         f"Cancelar a viagem #{v['id']} de {v['motorista']}?\n\nO veículo voltará ao status Disponível."):
            return
        try:
            db_vi.cancelar(v["id"], db_vi.buscar_veiculo_da_viagem(v["id"]))
            mostrar_sucesso("Cancelada", "Viagem cancelada. Veículo disponível novamente.")
            self.atualizar()
        except Exception as e:
            mostrar_erro("Erro", str(e))