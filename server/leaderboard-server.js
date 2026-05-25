// ── AdEmpire Global Leaderboard Server ───────────────────────────────────────
// v0.8 — Node.js/Express microserver
// Deploy: Railway / Render / any Node host
//
// Usage:
//   npm install express cors
//   node leaderboard-server.js
//
// Endpoints:
//   GET  /          → health check {status:"ok",version:"0.8"}
//   GET  /leaderboard → [{rank,name,score,months}] top 10
//   POST /score      → {name, score, months} → saves to in-memory store (swap for DB later)
//
// Environment variables:
//   PORT     (default 3000)
//   API_KEY  (optional, checked against Authorization: Bearer <key>)
// ─────────────────────────────────────────────────────────────────────────────

const express = require('express');
const cors    = require('cors');
const app     = express();

app.use(cors());
app.use(express.json());

// In-memory store (replace with SQLite or a free DB for persistence)
let scores = [
  { name: 'AdKing',      score: 985000, months: 12 },
  { name: 'MediaWizard', score: 871200, months: 14 },
  { name: 'CampaignAce', score: 762500, months: 16 },
  { name: 'BrandGuru',   score: 654300, months: 18 },
  { name: 'AdEmpire',    score: 543200, months: 20 },
];

const MAX_ENTRIES = 500;
const API_KEY     = process.env.API_KEY || '';

function authCheck(req, res) {
  if (!API_KEY) return true;
  const auth = req.headers['authorization'] || '';
  if (auth !== `Bearer ${API_KEY}`) {
    res.status(401).json({ error: 'unauthorized' });
    return false;
  }
  return true;
}

function getLeaderboard() {
  return scores
    .slice()
    .sort((a, b) => b.score - a.score)
    .slice(0, 10)
    .map((e, i) => ({ rank: i + 1, name: e.name, score: e.score, months: e.months }));
}

// ── Health check ─────────────────────────────────────────────────────────────
app.get('/', (_, res) => res.json({ status: 'ok', version: '0.8', entries: scores.length }));

// ── GET leaderboard ───────────────────────────────────────────────────────────
app.get('/leaderboard', (req, res) => {
  res.json(getLeaderboard());
});

// ── POST score ────────────────────────────────────────────────────────────────
app.post('/score', (req, res) => {
  if (!authCheck(req, res)) return;
  const { name, score, months } = req.body;
  if (typeof name !== 'string' || name.length < 1 || name.length > 32)
    return res.status(400).json({ error: 'invalid name (1-32 chars)' });
  if (typeof score !== 'number' || score < 0 || score > 1e9)
    return res.status(400).json({ error: 'invalid score' });
  if (typeof months !== 'number' || months < 1 || months > 1200)
    return res.status(400).json({ error: 'invalid months' });

  // Sanitize name
  const safeName = name.replace(/[<>"'&]/g, '').trim();

  scores.push({ name: safeName, score: Math.floor(score), months: Math.floor(months) });
  if (scores.length > MAX_ENTRIES)
    scores = scores.sort((a, b) => b.score - a.score).slice(0, MAX_ENTRIES);

  const board = getLeaderboard();
  const myRank = board.findIndex(e => e.name === safeName && e.score === Math.floor(score));
  res.json({ ok: true, rank: myRank >= 0 ? myRank + 1 : null, leaderboard: board });
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => console.log(`AdEmpire Leaderboard server running on port ${PORT}`));
