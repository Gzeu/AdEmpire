# AdEmpire Leaderboard Server

Node.js/Express microserver for the global leaderboard feature (v0.8).

## Deploy on Railway (free)

```bash
# 1. Push this repo to GitHub (already done)
# 2. Go to railway.app → New Project → Deploy from GitHub
# 3. Select repo → set root directory to /server
# 4. Railway auto-detects Node.js and runs npm start
# 5. Copy the generated URL → paste in AdEmpire Settings panel
```

## Deploy on Render (free)

```bash
# 1. render.com → New Web Service → Connect GitHub
# 2. Root: server/
# 3. Build command: npm install
# 4. Start command: node leaderboard-server.js
# 5. Free tier URL: https://your-app.onrender.com
```

## Local development

```bash
cd server
npm install
node leaderboard-server.js
# Server runs on http://localhost:3000
```

## Endpoints

| Method | Path | Description |
|--------|------|-------------|
| GET | `/` | Health check |
| GET | `/leaderboard` | Top-10 scores |
| POST | `/score` | Submit `{name, score, months}` |

## Persistence

Currently in-memory (resets on restart). For persistence, swap the `scores` array with SQLite:

```bash
npm install better-sqlite3
# Then update leaderboard-server.js to use DB queries instead of the array
```
