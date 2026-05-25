#pragma once
#include <string>
#include <vector>
#include <functional>

// AgentInterface — stub for AI/LLM agent layer (v0.9+)
//
// Planned integrations:
//   - OpenAI GPT-4o via REST (campaign brief generator)
//   - Local LLM via llama.cpp (offline fallback)
//   - Perplexity API (real-time market analysis summaries)
//
// Usage (future):
//   AgentInterface agent;
//   agent.SetApiKey(std::getenv("OPENAI_API_KEY"));
//   std::string brief = agent.GenerateCampaignBrief(clientName, budget, targetAudience);
//   std::string insight = agent.AnalyzeMarket(marketState);

struct AgentConfig {
    std::string provider;       // "openai" | "perplexity" | "local"
    std::string model;          // "gpt-4o" | "llama3" | "sonar"
    std::string apiKey;         // loaded from env, never hardcoded
    float temperature = 0.7f;
    int maxTokens = 512;
};

class AgentInterface {
public:
    AgentInterface() = default;
    explicit AgentInterface(const AgentConfig& config) : m_config(config) {}

    // Set API key at runtime (load from env in main.cpp)
    void SetApiKey(const std::string& key) { m_config.apiKey = key; }
    void SetProvider(const std::string& provider) { m_config.provider = provider; }
    bool IsConfigured() const { return !m_config.apiKey.empty(); }

    // --- STUBS (not yet implemented) ---

    // Generate a campaign brief for a client
    // Returns empty string if agent not configured
    std::string GenerateCampaignBrief(
        const std::string& /*clientName*/,
        float /*budget*/,
        const std::string& /*targetAudience*/
    ) const {
        return ""; // TODO: implement in v0.9
    }

    // Summarize current market conditions in plain English
    std::string AnalyzeMarketConditions(
        const std::string& /*marketSummary*/
    ) const {
        return ""; // TODO: implement in v0.9
    }

    // Suggest optimal campaign type given client profile
    std::string SuggestCampaignType(
        const std::string& /*clientIndustry*/,
        float /*satisfactionScore*/
    ) const {
        return ""; // TODO: implement in v0.9
    }

    // Negotiate counteroffer explanation
    std::string ExplainNegotiation(
        float /*offeredBudget*/,
        float /*counterBudget*/
    ) const {
        return ""; // TODO: implement in v0.9
    }

private:
    AgentConfig m_config;
};
