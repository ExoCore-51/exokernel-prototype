/*
 * Simple server to serve the monitor and provide kernel.log data
 * Run with: node server.js
 */

const http = require('http');
const fs = require('fs');
const path = require('path');

const PORT = 3000;
const KERNEL_LOG_PATH = path.join(__dirname, '..', 'kernel', 'kernel.log');
const MONITOR_DIR = __dirname;

const MIME_TYPES = {
    '.html': 'text/html',
    '.css': 'text/css',
    '.js': 'application/javascript',
    '.json': 'application/json'
};

const server = http.createServer((req, res) => {
    // Enable CORS
    res.setHeader('Access-Control-Allow-Origin', '*');

    // API endpoint for kernel.log
    if (req.url === '/api/logs') {
        try {
            if (fs.existsSync(KERNEL_LOG_PATH)) {
                const data = fs.readFileSync(KERNEL_LOG_PATH, 'utf8');
                res.writeHead(200, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ success: true, data: data }));
            } else {
                res.writeHead(200, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ success: false, message: 'No log file yet. Run the CLI first!' }));
            }
        } catch (err) {
            res.writeHead(500, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify({ success: false, message: err.message }));
        }
        return;
    }

    // Serve static files
    let filePath = req.url === '/' ? '/index.html' : req.url;
    filePath = path.join(MONITOR_DIR, filePath);

    const ext = path.extname(filePath);
    const contentType = MIME_TYPES[ext] || 'text/plain';

    fs.readFile(filePath, (err, content) => {
        if (err) {
            res.writeHead(404);
            res.end('File not found');
        } else {
            res.writeHead(200, { 'Content-Type': contentType });
            res.end(content);
        }
    });
});

server.listen(PORT, () => {
    console.log('');
    console.log('========================================');
    console.log('  EXOKERNEL MONITOR SERVER');
    console.log('========================================');
    console.log(`  Open: http://localhost:${PORT}`);
    console.log('  Press Ctrl+C to stop');
    console.log('========================================');
    console.log('');
});
