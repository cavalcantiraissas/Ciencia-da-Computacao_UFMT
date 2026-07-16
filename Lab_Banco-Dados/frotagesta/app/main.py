"""
FrotaGesta — main.py
Execute: PYTHONPATH=. python3 main.py
"""
import sys
import customtkinter as ctk

try:
    import psycopg2
except ImportError:
    import tkinter.messagebox as mb
    mb.showerror("Dependência faltando",
                 "psycopg2 não encontrado.\nInstale com: pip3 install psycopg2-binary")
    sys.exit(1)

from db.conexao import get_conn
from ui.tela_dashboard  import TelaDashboard
from ui.tela_motoristas import TelaMotoristas
from ui.tela_veiculos   import TelaVeiculos
from ui.tela_operacao   import TelaOperacao


def testar_conexao():
    try:
        get_conn().close()
        return True
    except Exception as e:
        import tkinter.messagebox as mb
        mb.showerror("Erro de Conexão",
                     f"Não foi possível conectar ao banco 'frotagesta'.\n\nDetalhe: {e}")
        return False


class App(ctk.CTk):
    def __init__(self):
        super().__init__()
        ctk.set_appearance_mode("system")
        ctk.set_default_color_theme("blue")
        self.title("FrotaGesta")
        self.geometry("1180x700")
        self.minsize(980, 620)
        self._build()

    def _build(self):
        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        # ── Sidebar ───────────────────────────────────────────────────────────
        sb = ctk.CTkFrame(self, width=190, corner_radius=0,
                          fg_color=("#1e2433", "#141921"))
        sb.grid(row=0, column=0, sticky="nsew")
        sb.grid_propagate(False)
        sb.grid_rowconfigure(9, weight=1)

        ctk.CTkLabel(sb, text="FrotaGesta",
                     font=ctk.CTkFont(size=16, weight="bold"),
                     text_color="white", anchor="w",
                     ).grid(row=0, column=0, padx=18, pady=(22, 2), sticky="w")
        ctk.CTkLabel(sb, text="Controle de Frota",
                     font=ctk.CTkFont(size=10),
                     text_color="#6b7280", anchor="w",
                     ).grid(row=1, column=0, padx=18, pady=(0, 22), sticky="w")

        ctk.CTkFrame(sb, height=1, fg_color="#2d3748").grid(
            row=2, column=0, sticky="ew", padx=14, pady=(0, 10))

        self._telas   = {}
        self._btn_nav = {}

        nav = [
            ("dashboard",  "Dashboard"),
            ("motoristas", "Motoristas"),
            ("veiculos",   "Veículos"),
            ("operacao",   "Operação"),
        ]
        for i, (key, label) in enumerate(nav, start=3):
            btn = ctk.CTkButton(
                sb, text=label, anchor="w",
                font=ctk.CTkFont(size=13),
                height=38, corner_radius=6,
                fg_color="transparent",
                text_color=("#cbd5e1", "#cbd5e1"),
                hover_color=("#2d3748", "#2d3748"),
                command=lambda k=key: self._navegar(k),
            )
            btn.grid(row=i, column=0, padx=10, pady=2, sticky="ew")
            self._btn_nav[key] = btn

        ctk.CTkFrame(sb, height=1, fg_color="#2d3748").grid(
            row=9, column=0, sticky="ew", padx=14, pady=(10, 10))

        self._switch_tema = ctk.CTkSwitch(
            sb, text="Modo escuro",
            font=ctk.CTkFont(size=11),
            text_color="#9ca3af",
            progress_color="#3b82f6",
            command=self._alternar_tema,
        )
        self._switch_tema.grid(row=10, column=0, padx=18, pady=(0, 20), sticky="w")

        # ── Conteúdo ──────────────────────────────────────────────────────────
        self._area = ctk.CTkFrame(self, corner_radius=0,
                                   fg_color=("gray93", "gray12"))
        self._area.grid(row=0, column=1, sticky="nsew")
        self._area.grid_rowconfigure(0, weight=1)
        self._area.grid_columnconfigure(0, weight=1)

        self._tela_atual = None
        self._navegar("dashboard")

    def _navegar(self, key):
        for k, b in self._btn_nav.items():
            ativo = k == key
            b.configure(
                fg_color=("#2d3748" if ativo else "transparent"),
                font=ctk.CTkFont(size=13, weight="bold" if ativo else "normal"),
                text_color=("white" if ativo else "#cbd5e1"),
            )

        if self._tela_atual:
            self._tela_atual.grid_forget()

        if key not in self._telas:
            cls = {"dashboard": TelaDashboard, "motoristas": TelaMotoristas,
                   "veiculos": TelaVeiculos,   "operacao":   TelaOperacao}[key]
            t = cls(self._area)
            t.grid(row=0, column=0, sticky="nsew")
            self._telas[key] = t
        else:
            t = self._telas[key]
            t.grid(row=0, column=0, sticky="nsew")
            if hasattr(t, "atualizar"):
                t.atualizar()

        self._tela_atual = t

    def _alternar_tema(self):
        ctk.set_appearance_mode("dark" if self._switch_tema.get() else "light")


if __name__ == "__main__":
    if not testar_conexao():
        sys.exit(1)
    App().mainloop()
