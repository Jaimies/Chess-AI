#pragma once

struct AiSettings {
    const bool useThreading;
};

const AiSettings debugAiSettings{false};
const AiSettings prodAiSettings{true};
