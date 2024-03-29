# Análisis de funciones necesarioas para el port del módulo event

## xMBMasterPortEventInit

Esta función aparentemente se encarga de "inicializar el manejador de eventos" (_a mi parecer_). por lo que veo en la implementación original de rt-thread lo aue hacen es crear un manejador de eventos.

```c
    bool xMBMasterPortEventInit(void)
```

Es llamada en la función eMBMasterInit:

```c
    if (eStatus == MB_ENOERR)
    {
        if (!xMBMasterPortEventInit())
        {
            /* port dependent event module initalization failed. */
            eStatus = MB_EPORTERR;
        }
        else
        {
            eMBState = STATE_DISABLED;
        }
        /* initialize the OS resource for modbus master. */
        vMBMasterOsResInit();
    }
    return eStatus;
```

Cómo el sistema operativo de AiThinker trabaja los eventos de manera distinta y puede tener un manejador de eventos por cada tarea es preferible que en esta función se instancie una tarea manejadora de eventos  **static void vMBMastertPortEventManager(void *parameter)**.

Este manejador de eventos es una tarea que no hace nada ya que al parecer los eventos se esperan solo cuando se necesitan con las funciones **xMBMasterPortEventGet**, **eMBMasterWaitRequestFinish** y son enviados por **xMBMasterPortEventPost** (en las modificacione que hice hasta ahora se maneja solo con esta función porque en rt-thread aparentemente lo hacía repitiendo el mismo código de envío de eventos).

## xMBMasterPortEventGet

Esta finción es para iniciar "los recursos del sistema operativo para modbus master", por lo que veo en la implementación de rt-thread lo que hacen es inicializar un semáforo.

```c
    void vMBMasterOsResInit(void)
```

Es llamada al final de  **eMBMasterInit**.
