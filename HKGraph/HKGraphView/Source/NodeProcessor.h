#pragma once

#include "JuceHeader.h"
#include "Graph.h"
#include "GraphViewTheme.h"

struct NodeProcessor : public Graph::Node {
  Graph *m_graph;

  explicit NodeProcessor(Graph *graph) : m_graph(graph) {
  }

  NodeProcessor(
    Graph *graph,
    const std::string &name,
    std::uint32_t n_ins,
    std::uint32_t n_outs)
    : m_graph(graph) {
    m_id = graph->make_uuid();
    m_name = name;
    for (auto i = 0u; i < n_ins; ++i) {
      auto id = graph->make_uuid();
      m_ins[id] = Graph::Node::Pin{
        .m_id = id,
        .m_order = static_cast<std::uint32_t>(i),
        .m_kind = Graph::Node::PinKind::In,
        .m_owner_node_id = m_id,
      };
    }

    for (auto i = 0u; i < n_outs; ++i) {
      auto id = graph->make_uuid();
      m_outs[id] = Graph::Node::Pin{
        .m_id = id,
        .m_order = static_cast<std::uint32_t>(i),
        .m_kind = Graph::Node::PinKind::Out,
        .m_owner_node_id = m_id,
      };
    }
  }

  ~NodeProcessor() override = default;

  Graph::Node::Pin addInputPin() {
    auto id = m_graph->make_uuid();
    auto order = static_cast<std::uint32_t>(m_ins.size());
    Graph::Node::Pin pin{
      .m_id = id,
      .m_order = order,
      .m_kind = Graph::Node::PinKind::In,
      .m_owner_node_id = m_id,
    };
    m_ins[id] = pin;
    m_graph->m_pins[id] = pin;
    return pin;
  }

  bool removeInputPin(const uuid &pin_id) {
    auto it = m_ins.find(pin_id);
    if (it == m_ins.end()) {
      return false;
    }
    m_ins.erase(it);
    m_graph->m_pins.erase(pin_id);

    // Re-index remaining in pins order
    std::vector<Graph::Node::Pin*> sorted_pins;
    for (auto &[_, p] : m_ins) {
      sorted_pins.push_back(&p);
    }
    std::sort(sorted_pins.begin(), sorted_pins.end(),
              [](const auto *a, const auto *b) { return a->m_order < b->m_order; });
    for (size_t i = 0; i < sorted_pins.size(); ++i) {
      sorted_pins[i]->m_order = static_cast<std::uint32_t>(i);
      m_graph->m_pins[sorted_pins[i]->m_id] = *sorted_pins[i];
    }
    return true;
  }

  virtual bool hasDynamicInputs() const {
    return false;
  }

  [[nodiscard]] virtual std::string typeId() const = 0;

  virtual void saveState(juce::ValueTree &nodeTree) {
    juce::ignoreUnused(nodeTree);
  }

  virtual void restoreState(const juce::ValueTree &nodeTree) {
    juce::ignoreUnused(nodeTree);
  }

  virtual NodeProcessor *clone() = 0;

  virtual juce::Component *createEditor(const GraphViewTheme &theme) = 0;
};
