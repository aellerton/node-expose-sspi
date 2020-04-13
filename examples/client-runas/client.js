const { sso } = require('../..');
const yargs = require('yargs');
const dbg = require('debug');
const debug = dbg('node-expose-sspi:client');

const myArgv = yargs
  .usage('$0 [url]', 'Request a url (by default http://localhost:3000)')
  .option('url', {
    type: 'string',
    description: 'Absolute url',
    default: 'http://localhost:3000',
  })
  .option('target', {
    alias: 't',
    type: 'string',
    description: 'Specify the target name (SPN)',
  })
  .option('user', {
    alias: 'u',
    type: 'string',
    description: 'Run as user',
  })
  .option('password', {
    alias: 'p',
    type: 'string',
    description: 'user password',
  })
  .option('domain', {
    alias: 'd',
    type: 'string',
    description: 'the windows domain',
    default: 'LOCAL',
  })
  .help()
  .alias('h', 'help').argv;

async function main(argv) {
  debug('argv: ', argv);
  const url = argv.url;
  const client = new sso.Client();
  try {
    if (argv.user) {
      client.setCredentials(argv.domain, argv.user, argv.password);
    }
    if (argv.target) {
      client.setTargetName(argv.target);
    }
    const response = await client.fetch(url);
    const json = await response.json();
    console.log('json: ', json);
  } catch (e) {
    console.error(e);
  }
}

main(myArgv);
