<div align="center">

# ⚡ MemStand

### Timer de sistema de alta precisão e otimizador inteligente de memória para Windows

<p align="center">
  <a href="README.md">English</a> • <a href="README.tr.md">Türkçe</a> • <a href="README.fr.md">Français</a> • <a href="README.de.md">Deutsch</a> • <a href="README.es.md">Español</a> • <a href="README.it.md">Italiano</a> • <b>Português (Brasil)</b> • <a href="README.ru.md">Русский</a>
</p>

[![Release](https://img.shields.io/github/v/release/enixyazilim/MemStand?color=3b82f6&logo=github&style=flat-square)](https://github.com/enixyazilim/MemStand/releases)
[![Build Status](https://img.shields.io/github/actions/workflow/status/enixyazilim/MemStand/ci.yml?branch=main&label=CI&logo=githubactions&logoColor=white&style=flat-square)](https://github.com/enixyazilim/MemStand/actions)
[![Language](https://img.shields.io/badge/C%2B%2B-20%20(ISO)-00599C?logo=c%2B%2B&style=flat-square)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%20%7C%2011%20(x64)-0078D4?logo=windows&style=flat-square)](https://microsoft.com/windows)
[![Dependencies](https://img.shields.io/badge/Dependencies-Zero%20(Pure%20Win32)-10b981?style=flat-square)](#-especificações-técnicas)
[![Binary Size](https://img.shields.io/badge/Binary%20Size-%3C%20500%20KB-success?style=flat-square)](#-especificações-técnicas)
[![RAM Footprint](https://img.shields.io/badge/RAM%20Usage-~1.9%20MB-success?style=flat-square)](#-especificações-técnicas)
[![License](https://img.shields.io/badge/License-MIT-8b5cf6?style=flat-square)](LICENSE)

<br/>

**MemStand** é um otimizador de sistema Windows moderno, ultraleve e autônomo, desenvolvido em **C++20** puro pela **[Enix Yazılım](https://github.com/enixyazilim)**. Ele fixa a resolução global do timer do kernel do Windows em **0,50 ms** para eliminar a latência do mouse, os microtravamentos e as variações de tempo de quadro nos jogos competitivos, ao mesmo tempo que oferece gerenciamento de memória automatizado e sensível ao hardware.

Construído segundo uma filosofia rigorosa de **zero dependências externas**, o MemStand não exige runtime .NET, WebView2, Electron nem serviços em segundo plano. Ele roda silenciosamente na área de notificação com um conjunto de trabalho ocioso de apenas **~1,9 MB** e **0,00%** de uso de CPU.

</div>

---

## ⚡ Principais recursos

| Recurso | Descrição |
| :--- | :--- |
| **[▶] Interruptor principal** | Início/parada instantânea do motor em segundo plano com um clique |
| **[⚡] Timer de 0,50 ms** | Bloqueio da resolução do kernel abaixo de milissegundo + watchdog |
| **[🧠] Perfilamento automático** | Calcula dinamicamente os limiares ideais a partir da RAM física |
| **[🎯] Limpeza da lista de espera** | Limpeza segura do cache de espera sem travar os jogos |
| **[📊] Área de notificação dinâmica** | Percentual de RAM ao vivo, codificado por cor, na barra de tarefas |
| **[🛡️] Inicialização sem UAC** | Logon administrativo silencioso via API do Agendador de Tarefas |
| **[🌐] Atualizador nativo** | Verifica os releases do GitHub diretamente via WinHTTP |
| **[🌍] i18n modular** | Traduções da comunidade plugáveis (baseadas em JSON) |

---

## 🥊 Por que o MemStand? (comparação com utilitários legados)

| Recurso | Ferramentas legadas (ex. ISLC) | Suítes OEM (Razer etc.) | **MemStand (Enix Yazılım)** |
| :--- | :---: | :---: | :---: |
| **Linguagem de programação** | C# / .NET Framework | Electron / Chromium | **C++20 puro (padrão ISO)** |
| **Dependências externas** | Runtime .NET 4.x/6.x | Node.js, Chromium, serviços | **Nenhuma (Win32 nativo e API NT)** |
| **Tamanho do executável** | ~3 - 10 MB | 150 MB - 500 MB+ | **< 500 KB** |
| **Memória ociosa** | 40 MB - 80 MB | 200 MB - 800 MB | **~1,9 MB** |
| **Limiares sensíveis ao hardware** | ❌ Valores estáticos fixos no código | ❌ Limpezas cegas e agressivas | **✅ Perfilamento dinâmico (RAM / 2)** |
| **Watchdog do timer** | ⚠️ Sondagem básica | ❌ Inexistente | **✅ Watchdog que reafirma a resolução do kernel** |
| **Logon elevado sem UAC** | ⚠️ Criação de tarefa inconsistente | ❌ Inicia um serviço pesado | **✅ Integração limpa com o Agendador de Tarefas** |
| **Modo escuro nativo moderno** | ❌ WinForms dos anos 90 | ⚠️ Renderizado na web / Pesado | **✅ Win32 Deep Slate desenhado à mão** |
| **Verificador de atualizações interno** | ❌ Busca manual na web | ⚠️ Atualização automática em segundo plano | **✅ Verificador de releases nativo via WinHTTP** |

---

## 🚀 Aprofundando: capacidades principais

### 1. ⚡ Perfilamento de memória sensível ao hardware
Diferente das ferramentas legadas, que exigem que o usuário chute valores arbitrários em megabytes, o MemStand inspeciona a RAM física instalada na inicialização e calcula os limiares ideais:
- **Limiar de espera (MB):** definido automaticamente para 50% da memória instalada (`RAM / 2`). Isso preserva o cache do sistema de arquivos do Windows, útil durante o uso normal, e garante que páginas de espera infladas nunca privem os jogos em execução de memória.
- **Margem de segurança de RAM livre:** escala dinamicamente conforme a faixa do seu hardware (8 GB, 16 GB, 24 GB, 32 GB, 64 GB+) para evitar esgotamento de memória.
- **Botão `⚡ Calcular automaticamente`:** restaura ou recalcula a qualquer momento os parâmetros recomendados para o seu hardware, com um único clique.

### 2. 🎮 Timer de kernel sub-milissegundo a 0,50 ms + watchdog
- Chama rotinas nativas do kernel NT (`NtSetTimerResolution`) combinadas com períodos de timer multimídia, reduzindo a resolução de 15,6 ms ou 1,00 ms para **0,50 ms** (5.000 unidades com precisão de 100 ns).
- Achata significativamente a variação de tempo de quadro, reduz o jitter da sondagem do mouse e melhora a responsividade no registro de acertos em títulos competitivos (*Valorant, CS2, Apex Legends, Warzone, Fortnite*).
- **Watchdog ativo:** reafirma continuamente a resolução em segundo plano para impedir que o Windows ou programas em segundo plano a redefinam silenciosamente para o padrão. Alvos configuráveis (0,50 ms, 0,75 ms, 1,00 ms, 15,62 ms).

### 3. 🧠 Limpeza de memória adaptativa em vários níveis
- **Nível 1 (carga normal / em jogo):** apenas a lista de espera é limpa com segurança. Os conjuntos de trabalho dos processos ativos permanecem quentes no cache, garantindo zero microtravamentos ou quedas de quadro.
- **Nível 2 (alta pressão de memória):** quando o uso total de RAM do sistema ultrapassa o limiar configurado (padrão: **85%**), os conjuntos de trabalho e o cache de arquivos do sistema são aliviados suavemente para evitar travamentos por falta de memória.

### 4. 📊 Selo dinâmico na área de notificação
- Desenha o percentual de uso de RAM em tempo real diretamente no ícone de notificação da barra de tarefas, usando buffer duplo GDI próprio.
- Codificado por cor de relance:
  - 🟢 **Verde esmeralda:** carga normal (< 70%)
  - 🟡 **Laranja âmbar:** carga moderada (70% - 85%)
  - 🔴 **Vermelho carmim:** alta pressão (> 85%)
- O menu de contexto da área de notificação oferece limpezas manuais em um clique, alternância do timer e verificação de atualizações, sem abrir a janela principal.

### 5. 🛡️ Inicialização elevada sem UAC
- Registra uma Tarefa Agendada nativa do Windows (`schtasks.exe` com nível de execução `HIGHEST`).
- Inicia automaticamente no logon do usuário com privilégios administrativos — eliminando por completo as solicitações de UAC do Windows na inicialização.

### 6. 🔄 Verificador nativo de releases e atualizações
- Um verificador de atualizações leve e integrado consulta a API de Releases do GitHub via `winhttp.dll`.
- Verifica as tags de release de forma assíncrona, sem bloquear a interface do usuário e sem exigir bibliotecas de terceiros.
- Acesso em um clique para baixar novas versões e consultar os changelogs.

---

## 📥 Download e instalação

O MemStand é **100% portátil**: não exige instalador nem modificações no registro.

1. Baixe o último `MemStand-vX.X.X-windows-x64.zip` em [GitHub Releases](https://github.com/enixyazilim/MemStand/releases/latest).
2. Extraia o arquivo para a pasta de sua preferência (por exemplo `C:\Tools\MemStand` ou `Desktop`).
3. Execute `MemStand.exe`.
4. *(Opcional)* Na aba **Configurações**, marque **Iniciar com o Windows (sem UAC)** e **Iniciar minimizado na bandeja** para uma operação silenciosa em segundo plano.

### Verificação (SHA-256)
Toda versão inclui um arquivo `checksums.txt`. Você pode verificar a integridade do download com o PowerShell:
```powershell
Get-FileHash -Path MemStand.exe -Algorithm SHA256
```

---

## 💻 Interface de linha de comando (CLI)

O MemStand aceita parâmetros de linha de comando para scripts, lançadores de jogos e tarefas em lote automatizadas:

| Argumento | Descrição |
| :--- | :--- |
| `MemStand.exe -minimized` | Inicia silenciosamente na bandeja, sem exibir a janela. |
| `MemStand.exe -purge` | Limpa imediatamente a lista de espera e sai silenciosamente. |
| `MemStand.exe -clean` | Executa uma varredura completa de memória (espera, conjuntos de trabalho, cache) e sai silenciosamente. |

---

## ⌨️ Atalho global

- **`Ctrl + Shift + M`**: dispara uma limpeza imediata de memória a partir de qualquer jogo ou aplicativo em tela cheia, sem alternar para a área de trabalho. Pode ser ligado e desligado na aba Configurações.

### 🎹 Atalho reconfigurável

Você não precisa ficar preso ao `Ctrl + Shift + M`. Marque **"Atalho global de limpeza"** na aba Configurações e clique no botão ao lado: o gravador arma (a borda fica âmbar e o rótulo muda para *Gravando...*), e então pressione a combinação que quiser.

- Pressione **`Esc`** durante a gravação para cancelar, ou **`Del`** para restaurar o `Ctrl + Shift + M` original.
- É exigida pelo menos uma entre `Ctrl`, `Alt`, `Shift` ou `Win`, para que o aplicativo nunca capture uma tecla isolada.
- A escolha só é aplicada quando você pressiona **Salvar configurações** e sobrevive a reinicializações.
- Se outro aplicativo já possuir essa combinação, o MemStand avisa em vez de falhar silenciosamente, e você pode escolher outra.
- A repetição automática de teclas é suprimida, portanto manter a combinação pressionada não enfileira limpezas repetidas.

---

## 🌐 Traduções da comunidade (i18n)

O MemStand é distribuído com **8 idiomas de interface** — English, Türkçe, Deutsch, Español, Français, Italiano, Português (Brasil) e Русский — e aceita um número ilimitado de traduções da comunidade por meio de arquivos JSON modulares.

A lista é ordenada alfabeticamente pelo nome nativo do idioma, e cada arquivo da pasta `languages/` é detectado automaticamente na inicialização.

1. Copie `languages/template.json` para `languages/<seu-idioma>.json` (por exemplo `languages/french.json`).
2. Traduza os valores do bloco `"strings"`. Mantenha os rótulos curtos — a tela de configurações usa colunas de largura fixa, então um rótulo longo invade o campo ao lado.
3. Reinicie o MemStand — seu idioma aparece imediatamente no menu **Idioma**.
4. Abra um Pull Request para compartilhar sua tradução com a comunidade!

---

## 📊 Especificações técnicas

| Parâmetro | Especificação |
| :--- | :--- |
| **Linguagem e padrão** | C++ puro (ISO C++20) |
| **Arquitetura** | Nativa de 64 bits (`x86_64`) |
| **APIs usadas** | API Win32, API Native NT (`ntdll.dll`), WinHTTP (`winhttp.dll`) |
| **Dependências externas** | **Nenhuma** (zero dependências em tempo de execução, binário autônomo) |
| **Tamanho do binário** | **~500 KB** (`MemStand.exe`) |
| **Consumo de memória** | **~1,9 MB** de conjunto de trabalho ocioso |
| **Uso de CPU** | **0,00%** entre ciclos de sondagem de um segundo |
| **Sistemas operacionais suportados** | Windows 10 (todas as edições a partir da 1809), Windows 11 (todas as edições) |

---

## 🛠️ Compilando a partir do código-fonte

### Pré-requisitos
- Windows 10 ou Windows 11 (x64)
- Visual Studio 2022 ou Visual Studio 2026 com a carga de trabalho *"Desenvolvimento de Área de Trabalho com C++"*
- CMake 3.20 ou mais recente (opcional, se compilando via CMake)

### Método 1: script de compilação com um clique
Execute o script de build no diretório raiz:
```cmd
build.bat
```
O executável autônomo compilado é gerado em `bin\MemStand.exe`.

### Método 2: CMake
```cmd
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

---

## 🤝 Contribuindo

Contribuições, relatórios de bugs e sugestões são bem-vindos! Consulte o [CONTRIBUTING.md](CONTRIBUTING.md) para as diretrizes detalhadas de estilo de código, convenções de arquitetura e fluxo de trabalho de Pull Requests.

---

## 📄 Licença

Este projeto é distribuído sob a [licença MIT](LICENSE).  
Copyright © 2026 **[Enix Yazılım](https://github.com/enixyazilim)** e os colaboradores do MemStand.
