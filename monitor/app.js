/* =============================================================================
   EXOKERNEL MONITOR - APPLICATION LOGIC
   ============================================================================= */

// Global state
let events = [];
let bindings = new Map(); // key: "app_id:page", value: binding object
let lastLoadedFile = null;

// DOM Elements
const syscallCount = document.getElementById('syscall-count');
const bindingCount = document.getElementById('binding-count');
const eventCount = document.getElementById('event-count');
const freePages = document.getElementById('free-pages');
const bindingsBody = document.getElementById('bindings-body');
const logContainer = document.getElementById('log-container');
const statusBadge = document.getElementById('status-badge');
const statusText = document.getElementById('status-text');

/**
 * Load and parse a kernel.log file
 */
function loadLogFile(event) {
    const file = event.target.files[0];
    if (!file) return;

    lastLoadedFile = file;

    const reader = new FileReader();
    reader.onload = function (e) {
        const content = e.target.result;
        parseLogContent(content);
        updateStatus('active', `Loaded: ${file.name}`);
    };
    reader.readAsText(file);
}

/**
 * Reload the last loaded file
 */
function reloadFile() {
    if (lastLoadedFile) {
        const reader = new FileReader();
        reader.onload = function (e) {
            events = [];
            bindings.clear();
            parseLogContent(e.target.result);
            updateStatus('active', `Reloaded: ${lastLoadedFile.name}`);
        };
        reader.readAsText(lastLoadedFile);
    } else {
        alert('No file loaded yet. Please load a kernel.log file first.');
    }
}

/**
 * Parse log content (newline-separated JSON)
 */
function parseLogContent(content) {
    const lines = content.trim().split('\n');

    lines.forEach(line => {
        try {
            const event = JSON.parse(line);
            processEvent(event);
        } catch (e) {
            console.warn('Failed to parse line:', line);
        }
    });

    updateDisplay();
}

/**
 * Process a single event
 */
function processEvent(event) {
    events.push(event);

    // Track bindings
    if (event.event === 'bind' || event.event === 'alloc_and_bind') {
        if (event.status === 'SUCCESS' || event.status === 'OK') {
            const key = `${event.app_id}:${event.page}`;
            bindings.set(key, {
                app_id: event.app_id,
                page: event.page,
                perms: event.perms,
                status: 'active'
            });
        }
    }

    if (event.event === 'unbind' || event.event === 'free_and_unbind') {
        if (event.status === 'SUCCESS' || event.status === 'OK') {
            const key = `${event.app_id}:${event.page}`;
            bindings.delete(key);
        }
    }
}

/**
 * Update all display elements
 */
function updateDisplay() {
    // Update stats
    eventCount.textContent = events.length;
    bindingCount.textContent = bindings.size;

    // Get syscall count from last kernel event
    const kernelEvents = events.filter(e => e.component === 'kernel');
    if (kernelEvents.length > 0) {
        const last = kernelEvents[kernelEvents.length - 1];
        if (last.count !== undefined) {
            syscallCount.textContent = last.count;
        }
    }

    // Update bindings table
    updateBindingsTable();

    // Update event log
    updateEventLog();
}

/**
 * Update bindings table
 */
function updateBindingsTable() {
    if (bindings.size === 0) {
        bindingsBody.innerHTML = `
            <tr class="empty-row">
                <td colspan="4">No active bindings</td>
            </tr>`;
        return;
    }

    let html = '';
    bindings.forEach((binding, key) => {
        const perms = formatPermissions(binding.perms);
        html += `
            <tr>
                <td><strong>${binding.app_id}</strong></td>
                <td>${binding.page}</td>
                <td>${perms}</td>
                <td><span class="perm-badge perm-r">Active</span></td>
            </tr>`;
    });
    bindingsBody.innerHTML = html;
}

/**
 * Format permissions as badges
 */
function formatPermissions(perms) {
    if (!perms) return '<span class="perm-badge">---</span>';

    let html = '';
    if (perms.includes('R')) html += '<span class="perm-badge perm-r">R</span> ';
    if (perms.includes('W')) html += '<span class="perm-badge perm-w">W</span> ';
    if (perms.includes('X')) html += '<span class="perm-badge perm-x">X</span> ';
    return html || '<span class="perm-badge">---</span>';
}

/**
 * Update event log
 */
function updateEventLog() {
    if (events.length === 0) {
        logContainer.innerHTML = `
            <div class="log-entry log-info">
                <span class="log-time">--:--:--</span>
                <span class="log-component">[monitor]</span>
                <span class="log-message">Waiting for events... Load a kernel.log file to begin.</span>
            </div>`;
        return;
    }

    let html = '';
    // Show last 100 events (most recent first)
    const recentEvents = events.slice(-100).reverse();

    recentEvents.forEach(event => {
        const time = formatTime(event.ts);
        const component = event.component || 'unknown';
        const message = formatEventMessage(event);
        const logClass = getLogClass(event.status);

        html += `
            <div class="log-entry ${logClass}">
                <span class="log-time">${time}</span>
                <span class="log-component">[${component}]</span>
                <span class="log-message">${message}</span>
            </div>`;
    });

    logContainer.innerHTML = html;
}

/**
 * Format timestamp
 */
function formatTime(ts) {
    if (!ts) return '--:--:--';
    const date = new Date(ts * 1000);
    return date.toLocaleTimeString();
}

/**
 * Format event message
 */
function formatEventMessage(event) {
    const action = event.event || 'unknown';
    const status = event.status || '';
    const msg = event.message || '';

    let text = `${action}`;
    if (event.app_id) text += ` app=${event.app_id}`;
    if (event.page) text += ` page=${event.page}`;
    if (event.perms && event.perms !== '---') text += ` perms=${event.perms}`;
    text += ` → ${status}`;
    if (msg) text += `: ${msg}`;

    return text;
}

/**
 * Get log entry CSS class based on status
 */
function getLogClass(status) {
    if (!status) return '';
    status = status.toUpperCase();
    if (status === 'SUCCESS' || status === 'OK') return 'log-success';
    if (status === 'ERROR' || status === 'DENIED') return 'log-error';
    if (status === 'WARNING') return 'log-warning';
    return 'log-info';
}

/**
 * Update status badge
 */
function updateStatus(state, text) {
    const dot = statusBadge.querySelector('.status-dot');
    if (state === 'active') {
        dot.classList.add('active');
    } else {
        dot.classList.remove('active');
    }
    statusText.textContent = text;
}

/**
 * Clear bindings display
 */
function clearBindings() {
    bindings.clear();
    updateBindingsTable();
    bindingCount.textContent = '0';
}

/**
 * Clear events display
 */
function clearEvents() {
    events = [];
    updateEventLog();
    eventCount.textContent = '0';
    syscallCount.textContent = '0';
}

// Auto-fetch from server
let autoFetchInterval = null;
let lastLogContent = '';

/**
 * Fetch logs from server API
 */
async function fetchLogsFromServer() {
    try {
        const response = await fetch('/api/logs');
        const result = await response.json();

        if (result.success && result.data !== lastLogContent) {
            lastLogContent = result.data;
            events = [];
            bindings.clear();
            parseLogContent(result.data);
            updateStatus('active', 'Live: Auto-updating');
        }
    } catch (err) {
        // Server not running, use file input instead
        console.log('Server not available, use file input.');
    }
}

/**
 * Start auto-fetch polling
 */
function startAutoFetch() {
    if (autoFetchInterval) return;

    fetchLogsFromServer(); // Fetch immediately
    autoFetchInterval = setInterval(fetchLogsFromServer, 2000); // Poll every 2 seconds
    updateStatus('active', 'Live: Connecting...');
}

/**
 * Stop auto-fetch polling
 */
function stopAutoFetch() {
    if (autoFetchInterval) {
        clearInterval(autoFetchInterval);
        autoFetchInterval = null;
    }
}

// Initialize - try auto-fetch first
console.log('Exokernel Monitor loaded.');
startAutoFetch();
