/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// See:
//
// https://searchfox.org/mozilla-central/source/devtools/client/shared/browser-loader.js
// https://searchfox.org/mozilla-central/source/devtools/shared/base-loader.js

"use strict";

var EXPORTED_SYMBOLS = ["UrlbarNpmLoader"];

const { XPCOMUtils } = ChromeUtils.import(
  "resource://gre/modules/XPCOMUtils.jsm"
);
XPCOMUtils.defineLazyModuleGetters(this, {
  Loader: "resource://devtools/shared/base-loader.js",
  Module: "resource://devtools/shared/base-loader.js",
  OS: "resource://gre/modules/osfile.jsm",
  Require: "resource://devtools/shared/base-loader.js",
  Services: "resource://gre/modules/Services.jsm",
});

function joinURI(initialPath, ...paths) {
  let url;
  try {
    url = new URL(initialPath);
  } catch (e) {
    return null;
  }
  for (const path of paths) {
    if (path) {
      url = new URL(path, url);
    }
  }
  return url.href;
}

function UrlbarNpmLoader(options) {
  const { loader, require } = new UrlbarNpmLoaderBuilder(options);
  return {
    loader,
    require,
  };
}

function UrlbarNpmLoaderBuilder({
  baseURI,
  window,
  useOnlyShared,
  commonLibRequire,
}) {
  const opts = {
    sandboxPrototype: window,
    sandboxName: "UrlbarNpmLoader",
    paths: {},
    requireHook: (id, require) => {
      console.log(
        `***XXXadw requireHook start, id=`, id,
        `require.resolve(id)=`, require.resolve(id)
      );

      switch (id) {
        case "escodegen":
        case "esprima":
        case "http":
        case "http-proxy-agent":
        case "https":
        case "https-proxy-agent":
        case "querystring":
        case "readline":
        case "url":
          return {};

        case "fs": {
          return {
            existsSync(path) {
              let file = Services.dirsvc.get("ProfD", Ci.nsIFile);
              let p = OS.Path.join(file.path, path);
              p = OS.Path.normalize(p);
              let file2 =
                Cc["@mozilla.org/file/local;1"].createInstance(Ci.nsIFile);
              try {
                file2.initWithPath(p);
                return file2.exists();
              } catch (e) {}
              return false;
            },
            // fs.writeFile(file, data[, options], callback)
            async writeFile(path, data, format, callback) {
              try {
                await OS.File.writeAtomic(path, data);
              } catch (ex) {
                callback(ex);
                return;
              }
              callback();
            },
            // fs.readFile(path[, options], callback)
            async readFile(path, ...rest) {
              console.log(`***XXXadw fs.readFile, path=`, path, `rest=`, rest);
              let callback = typeof rest[0] == "function" ? rest[0] : rest[1];
//               throw new Error("XXXadw");

              let match = /^\.\/node_modules\/(.+)/.exec(path);
              if (match) {
                //XXXadw doesn't work
                path = "resource:///modules/" + match[1];
                console.log(`***XXXadw fs.readFile, resource path=`, path);
                const rph = Services.io
                  .getProtocolHandler("resource")
                  .QueryInterface(Ci.nsIResProtocolHandler);
                path = rph.resolveURI(Services.io.newURI(path)).replace("file://", "");
                console.log(`***XXXadw fs.readFile, final path=`, path);
              }

              let buffer;
              try {
                buffer = await OS.File.read(path);
              } catch (ex) {
                console.log(`***XXXadw fs.readFile error! path=`, path, `ex=`, ex.toString());
                callback(ex);
                return;
              }
              callback(null, buffer);
            },
          };
        }

        case "path": {
          return {
            isAbsolute(path) {
              return path.length && path[0] == "/";
            },
            join(...paths) {
              return OS.Path.join(...paths);
            },
            normalize(path) {
              let file = Services.dirsvc.get("ProfD", Ci.nsIFile);
              let p = OS.Path.join(file.path, path);
              return OS.Path.normalize(p);
            },
          };
        }

        case "pino": {
          let pino = () => {};
          pino.debug = (...args) => console.debug(...args);
          pino.info = (...args) => console.info(...args);
          pino.warn = (...args) => console.warn(...args);
          pino.error = (...args) => console.error(...args);
          pino.trace = (...args) => console.trace(...args);
          pino.fatal = (...args) => console.error(...args);
          return pino;
        }
      }

      let uri;
//       if (id.startsWith("@")) {
      if (id.startsWith("@") || id.startsWith("kuromoji")) {
        uri = joinURI(baseURI, id.replace(/^@/, ""));
      } else if (id.startsWith("zlib")) {
        uri = joinURI(baseURI, "zlibjs");
      } else if (id.startsWith("async")) {
        uri = joinURI(baseURI, "async");
      } else if (id.startsWith("doublearray")) {
        uri = joinURI(baseURI, "doublearray");
      } else {

        let match = /^.*@nlpjs(.+)$/.exec(id);
        if (match) {
//           uri = joinURI(baseURI, "modules", "nlpjs", match[1]);
          uri = joinURI(baseURI, "nlpjs" + match[1]);
        } else {

          if (id.includes("keigo")) {
            // `./keigo` => nlpjs/lang-ja/src/keigo.json
            // require.resolve() automatically appends ".js" otherwise
            id += ".json";
          }

          uri = require.resolve(id);
        }
      }
      if (!uri.endsWith(".js") && !uri.endsWith(".json")) {
        //XXX this is actually declared in ${uri}/package.json in the `main` key
        if (uri.includes("kuromoji")) {
          uri = joinURI(uri + "/", "src/kuromoji.js");
        }
        else if (uri.includes("zlib")) {
          uri = joinURI(uri + "/", "bin/node-zlib.js");
        }
        else if (uri.includes("async")) {
          uri = joinURI(uri + "/", "dist/async.js");
        }
        else if (uri.includes("doublearray")) {
          uri = joinURI(uri + "/", "doublearray.js");
//         }
//         else if (uri.includes("keigo")) {
//           // `./keigo` => nlpjs/lang-ja/src/keigo.json
//           // require.resolve() automatically appends ".js" otherwise
//           uri = joinURI(uri + "/", "keigo.json");
        } else {
          uri = joinURI(uri + "/", "src/index.js");
        }
      }
      return require(uri);
    },

    globals: {
      // Allow modules to use the window's console to ensure logs appear in a
      // tab toolbox, if one exists, instead of just the browser console.
      console: window.console,
      process: {
        cwd() {
          return Services.dirsvc.get("ProfD", Ci.nsIFile).path;
        },
        env: {
          NODE_ENV: "debug",
        },
        // process.nextTick(callback[, ...args])
        nextTick(callback, ...rest) {
          Services.tm.dispatchToMainThread(() => callback(...rest));
        },
      },

      //XXXadw used by resource:///modules/nlpjs/lang-ja/src/stemmer-ja.js to
      // load a (optional?) dictionary file, but it hardcodes `node_modules` in
      // the relatiive path!  => should use resource://modules/node_modules/ as
      // base uri?  or resource://node_modules/
      __dirname: "resource://modules/",

      //XXXadw used by resource:///modules/zlibjs/bin/node-zlib.js
      Buffer: Uint8Array,
    },
  };
  const mainModule = Module(baseURI, joinURI(baseURI, "main.js"));
  this.loader = Loader(opts);
  this.require = Require(this.loader, mainModule);
}
