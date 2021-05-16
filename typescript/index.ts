import { Telegraf } from "telegraf";
import * as readline from "readline";

const runBot = async (botToken: string) => {
  const chatIds: number[] = [];

  const bot = new Telegraf(botToken);
  bot.start((ctx) => {
    ctx.reply(
      `Das hier ist ChatID "${ctx.chat.id}", ich melde mich wenn es klingelt`
    );

    chatIds.push(ctx.chat.id);
  });
  const done = bot.launch();

  // Enable graceful stop
  process.once("SIGINT", () => bot.stop("SIGINT"));
  process.once("SIGTERM", () => bot.stop("SIGTERM"));

  await done;

  console.log("Bot is up and running");

  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });

  rl.on("line", () => {
    console.log("Local Ding Dong");
    chatIds.forEach((chatId) => {
      bot.telegram.sendMessage(chatId, "Ding Dong");
    });
  });
};

const botToken = process.env.TELEGRAM_BOT_TOKEN;
if (!botToken) {
  console.error(`TELEGRAM_BOT_TOKEN must be present in ENV`);
} else {
  runBot(botToken);
}
