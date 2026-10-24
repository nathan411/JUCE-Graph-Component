#include "EdgeComponent.h"

EdgeComponent::EdgeComponent(
  const GraphViewTheme &viewTheme,
  NodeComponent::PinComponent *from,
  NodeComponent::PinComponent *to,
  Graph::Edge graphEdge) :
  theme(viewTheme),
  startPin(from),
  endPin(to), model(graphEdge) {
}

EdgeComponent::~EdgeComponent() = default;

bool EdgeComponent::hitTest(int x, int y) {
  auto position = juce::Point<float>(static_cast<float>(x), static_cast<float>(y));
  juce::Point<float> nearest;
  path.getNearestPoint(juce::Point<float>(static_cast<float>(x), static_cast<float>(y)), nearest);
  auto distance = position.getDistanceFrom(nearest);
  return distance < 5;
}

void EdgeComponent::paint(juce::Graphics &g) {
  auto w = static_cast<float>(getWidth());
  auto h = static_cast<float>(getHeight());

  // Connect from right edge of startPin (Out) to left edge of endPin (In)
  auto pstart = getLocalPoint(startPin, juce::Point<float>(static_cast<float>(startPin->getWidth()), static_cast<float>(startPin->getHeight()) * 0.5f));
  auto pend = getLocalPoint(endPin, juce::Point<float>(0.0f, static_cast<float>(endPin->getHeight()) * 0.5f));

  path.clear();

  path.startNewSubPath(pstart.x, pstart.y);
  if (inverted) {
    path.cubicTo(0.0f, pstart.y, w, pend.y, pend.x, pend.y);
  } else {
    path.cubicTo(w * 0.5f, pstart.y, w * 0.5f, pend.y, pend.x, pend.y);
  }

  if (selected) {
    g.setColour(juce::Colour(theme.cEdgeSelected));
  } else {
    g.setColour(juce::Colour(theme.cEdge));
  }

  g.strokePath(path, juce::PathStrokeType(theme.edgeStrokeWidth));

}

bool EdgeComponent::isConnecting(
  const NodeComponent::PinComponent *first,
  const NodeComponent::PinComponent *second) const {
  return (startPin == first && endPin == second) || (startPin == second && endPin == first);
}

void EdgeComponent::debug(const std::string &action) const {
  std::cout
    << "[EdgeComponent::" << action << "]"
    << " sourceNodeId = " << startPin->owner->m_processor->m_id
    << ", startPin = " << startPin->model.m_order
    << ", targetNodeId = " << endPin->owner->m_processor->m_id
    << ", endPin = " << endPin->model.m_order
    << std::endl;
}
