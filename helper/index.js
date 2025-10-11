const fs = require("fs");

const main = fs.readFileSync("main/main.c").toString();

const parts = main.split("\n/* XTENSA FORWARD DECLARATIONS */");

const args = process.argv.slice(2);
const header = fs.readFileSync(args[0]).toString();

const file_dir = header.split("\n")[0].slice(2).trim();

const src = fs.readFileSync("./src/" + file_dir).toString();
const ctx =
  parts[0] + header + "\n/* XTENSA FORWARD DECLARATIONS */" + parts[1] + src;

fs.writeFileSync("main/main.c", ctx);
