# Visão geral

Este arquivo Arduino configura um controlador de bateria eletrônica baseado na biblioteca `Edrumulus`. Ele realiza a leitura dos sensores dos pads, processa os impactos, controla o LED de status e envia ou recebe mensagens MIDI.

A diretiva `#define USE_MIDI` habilita os recursos MIDI. O código também pode ser compilado para diferentes plataformas, como ESP32 e Teensy, usando diretivas de pré-processamento como `ESP_PLATFORM`, `USE_TINYUSB` e `TEENSYDUINO`.

## Configuração dos pinos

Os arrays `analog_pins4` e `analog_pins_rimshot4` associam GPIOs aos pads da bateria. A ordem dos elementos é: caixa, bumbo, chimbal, controle do chimbal, crash, tom 1, ride, tom 2 e tom 3.

O valor `-1` indica que determinado canal não está disponível. A variável `number_pads4` está definida como `8`, portanto os oito primeiros pads são processados. Embora o tom 3 esteja configurado nas funções de preset, seu índice é `8` e fica fora do intervalo normalmente processado, que vai de `0` a `7`.

## Variáveis globais

O objeto `edrumulus` representa o controlador principal da biblioteca. A variável `midi_channel` define o canal MIDI usado, sendo o canal 10 o padrão para instrumentos de bateria.

As constantes `hihat_pad_idx` e `hihatctrl_pad_idx` identificam o pad do chimbal e o pedal de controle do chimbal. Esses índices são usados posteriormente para enviar o estado de abertura ou fechamento do chimbal.

As variáveis `number_pads`, `status_LED_pin`, `is_status_LED_on` e `selected_pad` armazenam, respectivamente, a quantidade ativa de pads, o pino do LED, o estado atual do LED e o pad selecionado para configuração.

## Inicialização com `setup()`

A função `setup()` é executada uma vez quando a placa é ligada. Primeiro, ela define os arrays de pinos padrão e chama `Edrumulus_hardware::get_prototype_pins()`. Essa função pode substituir os pinos, a quantidade de pads e o pino do LED de acordo com o protótipo de hardware detectado.

Em seguida, o pino do LED é configurado como saída e o LED é aceso para indicar que a inicialização está em andamento. Quando o modo de plotagem serial está habilitado no ESP32, a quantidade de pads é limitada a sete.

Depois, a comunicação MIDI e serial é inicializada. No ESP32, o código pode usar MIDI via TinyUSB ou a interface MIDI padrão. No Teensy, ele utiliza `usbMIDI`. O uso da macro `MYMIDI` permite que o restante do código utilize a mesma interface independentemente da plataforma.

Por fim, `edrumulus.setup()` configura o processamento dos pads. O LED é desligado após essa etapa. Em placas ESP32, são aplicadas configurações predefinidas por meio de `preset_settings()`. Em outras plataformas, as configurações são carregadas da memória persistente usando `read_settings()`.

## Configurações padrão

A função `preset_settings()` define os valores iniciais do controlador. Ela associa notas MIDI aos pads, como a nota 38 para a caixa e a nota 36 para o bumbo.

O chimbal possui configurações adicionais. Existe uma nota para o chimbal fechado, outra para o chimbal aberto e uma configuração separada para o pedal. Isso permite que o sintetizador MIDI diferencie diferentes posições e ações do chimbal.

A função também informa à biblioteca qual modelo de pad está conectado a cada entrada, usando tipos como `Pad::PD8`, `Pad::KD7`, `Pad::CY6` e `Pad::TP80`. O tipo do pad influencia a forma como o sinal analógico é interpretado.

## Processamento principal em `loop()`

A função `loop()` é executada continuamente. A chamada `edrumulus.process()` realiza a amostragem dos sensores e detecta impactos, controles, sobrecargas e erros. Como esse processamento precisa respeitar uma taxa de amostragem específica, a chamada pode bloquear a execução durante o processamento.

Depois disso, o código verifica se existe uma condição de sobrecarga ou erro. Quando isso acontece, o LED é aceso. O uso da variável `is_status_LED_on` evita escrever repetidamente no pino do LED enquanto o erro permanece ativo.

Se houver um erro de deslocamento DC, o código envia uma mensagem MIDI especial usando o número de nota `125`. Valores a partir de `64` identificam o canal ou a entrada com problema. Um valor `1` representa um erro geral. Quando o problema desaparece, o valor `0` informa que todos os erros foram limpos.

## Envio de notas MIDI

Para cada pad ativo, o código verifica se foi detectado um pico de sinal por meio de `get_peak_found()`. Quando um impacto é encontrado, ele obtém a velocidade e a nota MIDI correspondentes.

Se o pad usar sensibilidade posicional, o código envia um `Control Change` no controlador 16. Esse valor representa a posição do impacto no pad, permitindo que um sintetizador produza sons diferentes dependendo do local atingido.

O chimbal recebe tratamento especial. Antes de enviar a nota do chimbal, o código transmite o estado do pedal por meio de uma mensagem `Control Change`. Se o chimbal estiver aberto, a nota normal é substituída pela nota configurada para o chimbal aberto.

Cada impacto é enviado como uma mensagem `Note On`, seguida imediatamente por uma mensagem `Note Off`. Isso garante que o sintetizador receba um evento completo de nota.

## Controles e abafamento dos pratos

Além dos impactos, o código verifica se algum controle contínuo foi detectado. Nesse caso, ele obtém o número do controlador e seu valor e envia uma mensagem MIDI `Control Change`.

O recurso de choke dos pratos utiliza aftertouch polifônico. Quando a borda do prato é segurada, o código envia aftertouch com valor `127` para as notas normal, rim, aberta normal e aberta rim.

Existe uma exceção: quando a nota aberta do rim é configurada como `0`, o choke é representado por uma nova nota MIDI em vez de aftertouch. Quando a borda é liberada, o código envia aftertouch com valor `0`, indicando que o choke terminou.

## Recepção de configurações via MIDI

A segunda parte da função `loop()` recebe mensagens MIDI destinadas à configuração do controlador. `MYMIDI.read(midi_channel)` verifica se existe uma mensagem no canal configurado.

O código processa apenas mensagens do tipo `Control Change`. O número do controlador identifica qual configuração deve ser alterada, e o valor da mensagem contém o novo valor.

Os controladores de `102` a `122` configuram parâmetros como tipo do pad, limiar de velocidade, sensibilidade, detecção de rim shot, curva MIDI, cancelamento de crosstalk, notas MIDI, tempo de máscara, reforço do rim shot e acoplamento entre pads.

Cada alteração normalmente segue três etapas: o novo valor é aplicado ao objeto `edrumulus`, o valor é salvo usando `write_setting()` e uma mensagem de confirmação é enviada ao software de controle.

O controlador `108` seleciona o pad que será editado. O valor só é aceito quando é menor que `MAX_NUM_PADS`. Já o controlador `111` combina duas opções em um único valor:

- `0`: rim shot e sensibilidade posicional desativados;
- `1`: rim shot ativado e sensibilidade posicional desativada;
- `2`: rim shot desativado e sensibilidade posicional ativada;
- `3`: ambos ativados.

O controlador `115` restaura as configurações padrão e grava todos os valores na memória persistente.

## Confirmação das configurações

A função `confirm_setting()` envia respostas ao programa de configuração usando mensagens MIDI `Note Off`. Quando `send_all` é `true`, todos os parâmetros do pad selecionado são enviados novamente. Isso acontece, por exemplo, depois que um pad é selecionado ou quando seu tipo é alterado.

Quando `send_all` é `false`, somente o parâmetro modificado é retornado. Essa confirmação permite que a interface gráfica verifique se a alteração foi recebida e aplicada corretamente.

Os controladores `126` e `127` são usados para informar as versões menor e maior do firmware. O controlador `125` permanece reservado para mensagens de erro.

## Leitura das configurações

A função `read_settings()` percorre todos os pads ativos e restaura seus parâmetros a partir da memória persistente.

O tipo do pad é carregado primeiro porque `set_pad_type()` redefine outros parâmetros internos. Depois disso, são restaurados os limiares, sensibilidades, curva MIDI, suporte a rim shot, sensibilidade posicional, notas MIDI, cancelamento de crosstalk, tempo de máscara e demais configurações.

O nível global de cancelamento de picos é armazenado separadamente usando `number_pads` como índice do registro.

## Gravação das configurações

A função `write_all_settings()` realiza a operação inversa de `read_settings()`. Ela obtém cada valor atual por meio dos métodos `get_...()` e grava os dados usando `write_setting()`.

Cada parâmetro possui um índice fixo, de `0` a `18`, para cada pad. Depois de salvar todos os pads ativos, o nível global de cancelamento de picos é gravado separadamente.

Dessa forma, o programa mantém uma separação clara entre os valores usados em tempo de execução e os valores armazenados permanentemente. Em plataformas que não oferecem suporte à memória persistente, como indicado no código para o ESP32, os presets são usados diretamente durante a inicialização.
