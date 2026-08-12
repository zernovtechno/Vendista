[README НА РУССКОМ](./READMERUS.md)
# Vendista - a library for creating vending machine controllers

Learn more on the Wiki! (work-in-progress)

This library allows you to create your own unique vending machine controller (VMC) based on the Vendista terminal.
Features include payment requests, touch tracking, screen filling, text string rendering, and terminal reboot on command.

A variety of colors, currency codes, and terminal commands are available.
You can also read outgoing packets from the terminal, such as user commands sent to the Master Controller through the Vendista web panel.

## Why Vendista?

From personal experience, Vendista provides the most comprehensive set of tools for managing terminals in [slave mode](https://wiki.vendista.ru/en/home/slave_protocol).
Anyone with a relatively steady hand can launch this mode on the terminal and control it in any way: from a computer or a regular Arduino board.

However, it turned out that no one had created a library for working with Vendista terminals via UART before me. Instead, smart guys in glasses were asking 100-150 thousand rubles on very niche forums for an Arduino solution using MDB bus adapters.

But with a Slave mode that supports regular UART, this is completely unnecessary, which is why we're here.

## Why Arduino?

Arduino is, first and foremost, a platform. A platform that 99% of modern engineers use or have used.

Arduino is fast, simple, and really cool.

## Positive features of the library

1. All payment processing (requests to the MinPay server) remains on the terminal. This library (and its developer) are not responsible for your money or your customers' money; it only issues commands to the terminal.
2. By implementing user input in the Vendista Personal Account, you can control your Arduino project VIA the terminal, without having to install two SIM cards or connect both devices to a WiFi network.
3. It works on the ESP32/8266 and Arduino, but in theory it can work on any microcontroller that supports compiling with Arduino code.
4. It's very small, but saves a lot of time writing your own solution.

## Issues...

1. This is my first Arduino library, and I have no idea how to write them. I tried to follow [Gyver's instructions](https://alexgyver.ru/lessons/library-writing/), and you can judge for yourself how it turned out. I look forward to your pull requests, or at least pointing out any errors in my issues. 2. The code was originally written for personal use, with the goal of a simple and quick implementation. Therefore, in some places it's completely inflexible, non-asynchronous, and not optimal. I'll try to fix this in updates!
3. The code uses Delay and String. I know this is bad, but I'll definitely fix it one day. You can still disable Delay by specifying 0 in the appropriate place (see below). Oh, and the source code is available, so I welcome your suggestions for fixes and improvements to the library.

I hope this modest implementation will save someone time delving into the Vendista documentation, and also prevent you from using MDB adapters where they're clearly not needed.

For connection and usage instructions, see the [Wiki](https://github.com/zernovtechno/Vendista/wiki).
