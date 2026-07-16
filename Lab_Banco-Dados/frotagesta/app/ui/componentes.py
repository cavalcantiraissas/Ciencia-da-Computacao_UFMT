"""
FrotaGesta — ui/componentes.py
Widgets reutilizáveis em todas as telas.
"""

import customtkinter as ctk
import tkinter.messagebox as mb

# ── Paleta ────────────────────────────────────────────────────────────────────
COR_PRIMARIA    = "#1a56db"
COR_PRIM_HOVER  = "#1e429f"
COR_SUCESSO     = "#057a55"
COR_SUCS_HOVER  = "#046c4e"
COR_PERIGO      = "#c81e1e"
COR_PERI_HOVER  = "#9b1c1c"
COR_NEUTRO      = "#374151"
COR_NEUT_HOVER  = "#1f2937"

STATUS_CORES = {
    "Disponível":         ("#d1fae5", "#065f46"),
    "Em rota":            ("#dbeafe", "#1e40af"),
    "Manutenção Urgente": ("#fee2e2", "#991b1b"),
    "Em manutenção":      ("#fef3c7", "#92400e"),
    "Concluída":          ("#d1fae5", "#065f46"),
    "Em andamento":       ("#dbeafe", "#1e40af"),
    "Cancelada":          ("#fee2e2", "#991b1b"),
    "Pendente":           ("#fef3c7", "#92400e"),
    "Resolvido":          ("#d1fae5", "#065f46"),
}


def label_titulo(master, texto, tamanho=18):
    return ctk.CTkLabel(
        master, text=texto,
        font=ctk.CTkFont(family="Helvetica", size=tamanho, weight="bold"),
    )


def label_sub(master, texto, tamanho=11):
    return ctk.CTkLabel(
        master, text=texto,
        font=ctk.CTkFont(size=tamanho),
        text_color=("gray45", "gray55"),
    )


def label_secao(master, texto):
    return ctk.CTkLabel(
        master, text=texto.upper(),
        font=ctk.CTkFont(size=10, weight="bold"),
        text_color=("gray50", "gray50"),
    )


def btn_primario(master, texto, comando, largura=130, altura=34):
    return ctk.CTkButton(
        master, text=texto, command=comando,
        width=largura, height=altura,
        font=ctk.CTkFont(size=12, weight="bold"),
        fg_color=COR_PRIMARIA, hover_color=COR_PRIM_HOVER,
        corner_radius=6,
    )


def btn_sucesso(master, texto, comando, largura=130, altura=34):
    return ctk.CTkButton(
        master, text=texto, command=comando,
        width=largura, height=altura,
        font=ctk.CTkFont(size=12, weight="bold"),
        fg_color=COR_SUCESSO, hover_color=COR_SUCS_HOVER,
        corner_radius=6,
    )


def btn_perigo(master, texto, comando, largura=130, altura=34):
    return ctk.CTkButton(
        master, text=texto, command=comando,
        width=largura, height=altura,
        font=ctk.CTkFont(size=12, weight="bold"),
        fg_color=COR_PERIGO, hover_color=COR_PERI_HOVER,
        corner_radius=6,
    )


def btn_neutro(master, texto, comando, largura=130, altura=34):
    return ctk.CTkButton(
        master, text=texto, command=comando,
        width=largura, height=altura,
        font=ctk.CTkFont(size=12),
        fg_color=COR_NEUTRO, hover_color=COR_NEUT_HOVER,
        corner_radius=6,
    )


def btn_outline(master, texto, comando, largura=130, altura=34):
    return ctk.CTkButton(
        master, text=texto, command=comando,
        width=largura, height=altura,
        font=ctk.CTkFont(size=12),
        fg_color="transparent",
        border_width=1,
        border_color=("gray60", "gray50"),
        text_color=("gray20", "gray80"),
        hover_color=("gray88", "gray22"),
        corner_radius=6,
    )


def campo(master, placeholder="", largura=260, altura=34):
    return ctk.CTkEntry(
        master,
        placeholder_text=placeholder,
        width=largura, height=altura,
        font=ctk.CTkFont(size=12),
        corner_radius=6,
    )


def dropdown(master, valores, largura=260):
    var = ctk.StringVar(value=valores[0] if valores else "")
    cb = ctk.CTkComboBox(
        master, values=valores, variable=var,
        width=largura, height=34,
        font=ctk.CTkFont(size=12),
        state="readonly",
        corner_radius=6,
    )
    return cb, var


def card(master, **kwargs):
    return ctk.CTkFrame(master, corner_radius=8, **kwargs)


def separador(master, cor=("gray82", "gray28")):
    return ctk.CTkFrame(master, height=1, fg_color=cor)


def label_campo(master, texto):
    return ctk.CTkLabel(
        master, text=texto,
        font=ctk.CTkFont(size=11, weight="bold"),
        text_color=("gray30", "gray70"),
        anchor="w",
    )


def mostrar_erro(titulo, mensagem):
    mb.showerror(titulo, mensagem)


def mostrar_sucesso(titulo, mensagem):
    mb.showinfo(titulo, mensagem)


def confirmar(titulo, mensagem) -> bool:
    return mb.askyesno(titulo, mensagem)


class Tabela(ctk.CTkScrollableFrame):
    """
    Tabela com cabeçalho fixo, linhas zebradas e seleção visual.
    colunas: lista de (chave, título, largura_px)
    """

    COR_HEADER   = "#1e3a5f"
    COR_SEL_L    = "#bfdbfe"
    COR_SEL_D    = "#1e3a5f"

    def __init__(self, master, colunas, altura=360, **kwargs):
        super().__init__(master, height=altura, **kwargs)
        self.colunas = colunas
        self._linha_selecionada = None
        self._widgets_linhas = []
        self._on_click = None
        self._desenhar_header()

    def _desenhar_header(self):
        for col, (_, titulo, larg) in enumerate(self.colunas):
            ctk.CTkLabel(
                self, text=titulo,
                width=larg, height=30,
                font=ctk.CTkFont(size=11, weight="bold"),
                text_color="white",
                fg_color=self.COR_HEADER,
                corner_radius=0,
                anchor="w",
                padx=8,
            ).grid(row=0, column=col, padx=(0, 1), pady=(0, 1), sticky="ew")

    def popular(self, dados, on_click=None):
        self._on_click = on_click
        self._linha_selecionada = None
        self._widgets_linhas = []

        for widget in self.winfo_children():
            info = widget.grid_info()
            if info and int(info["row"]) > 0:
                widget.destroy()

        for i, linha in enumerate(dados):
            bg_l = "gray96" if i % 2 == 0 else "white"
            bg_d = "gray19" if i % 2 == 0 else "gray16"
            widgets_row = []

            for col, (chave, _, larg) in enumerate(self.colunas):
                valor = linha.get(chave, "")
                valor_str = str(valor) if valor is not None else "—"

                if chave == "status" and valor_str in STATUS_CORES:
                    fg_l, fg_d = STATUS_CORES[valor_str]
                    lbl = ctk.CTkLabel(
                        self, text=valor_str,
                        width=larg, height=28,
                        font=ctk.CTkFont(size=10, weight="bold"),
                        fg_color=(fg_l, fg_l),
                        text_color=(fg_d, fg_d),
                        corner_radius=4,
                        anchor="center",
                    )
                else:
                    lbl = ctk.CTkLabel(
                        self, text=valor_str,
                        width=larg, height=28,
                        font=ctk.CTkFont(size=11),
                        fg_color=(bg_l, bg_d),
                        corner_radius=0,
                        anchor="w",
                        padx=8,
                    )

                lbl.grid(row=i + 1, column=col, padx=(0, 1), pady=(0, 1), sticky="ew")
                lbl._bg_l = bg_l
                lbl._bg_d = bg_d
                lbl._chave = chave
                lbl._valor = valor_str

                if on_click:
                    lbl.bind("<Button-1>", lambda e, idx=i, d=linha: self._handle_click(idx, d))
                    lbl.configure(cursor="hand2")

                widgets_row.append(lbl)
            self._widgets_linhas.append(widgets_row)

    def _handle_click(self, idx, dado):
        # Deseleciona linha anterior
        if self._linha_selecionada is not None and self._linha_selecionada < len(self._widgets_linhas):
            for lbl in self._widgets_linhas[self._linha_selecionada]:
                if lbl._chave != "status" or lbl._valor not in STATUS_CORES:
                    lbl.configure(fg_color=(lbl._bg_l, lbl._bg_d))

        # Seleciona nova linha
        self._linha_selecionada = idx
        for lbl in self._widgets_linhas[idx]:
            if lbl._chave != "status" or lbl._valor not in STATUS_CORES:
                lbl.configure(fg_color=(self.COR_SEL_L, self.COR_SEL_D))

        if self._on_click:
            self._on_click(dado)