#include "draw_queue.hpp"

// Avoid dynamic alloc that we would get with std::function
// Also I enjoy this kinda shit
struct DrawQueue::QueueElement {
    class Callable {
    public:
        virtual ~Callable() = default;
        virtual void operator()(ImDrawList* draw_list) = 0;
        virtual Callable* move_into(char* memory) = 0;
    };
    template<class FunT>
    class CallableImpl : public Callable {
    public:
        CallableImpl(FunT fun) : Function{ std::move(fun) } {}
        virtual void operator()(ImDrawList* draw_list) { Function(draw_list); }
        virtual Callable* move_into(char* memory) { return new(memory) CallableImpl<FunT>{ std::move(Function) }; };
    private:
        FunT Function;
    };

    template<class FunT>
    QueueElement(FunT fun)
    {
        static_assert(sizeof(FunT) <= sizeof(Storage), "Storage does not fit functor, try to increase the storage size");
        Function = new(Storage) CallableImpl<std::decay_t<FunT>>{ std::move(fun) };
    }
    ~QueueElement()
    {
        Function->~Callable();
    }

    QueueElement(QueueElement&& rhs) noexcept { *this = std::move(rhs); }
    QueueElement& operator=(QueueElement&& rhs) noexcept
    {
        Function = rhs.Function->move_into(Storage);
        return *this;
    }

    QueueElement(const QueueElement& rhs) = delete;
    QueueElement& operator=(const QueueElement& rhs) = delete;

    void operator()(ImDrawList* draw_list)
    {
        Function->operator()(draw_list);
    }

    char Storage[64];
    Callable* Function;
};

DrawQueue::DrawQueue() = default;
DrawQueue::~DrawQueue() = default;

void DrawQueue::queue_line(ImVec2 start, ImVec2 end, float thickness, ImU32 color)
{
    m_Queue.emplace_back([=](ImDrawList* draw_list) {
        draw_list->AddLine(start, end, color, thickness);
    });
}
void DrawQueue::queue_rect(ImVec2 min, ImVec2 max, float thickness, float rounding, ImU32 color)
{
    m_Queue.emplace_back([=](ImDrawList* draw_list) {
        draw_list->AddRect(min, max, color, rounding, ImDrawCornerFlags_All, thickness);
    });
}
void DrawQueue::queue_rect_filled(ImVec2 min, ImVec2 max, float rounding, ImU32 color)
{
    m_Queue.emplace_back([=](ImDrawList* draw_list) {
        draw_list->AddRectFilled(min, max, color, rounding, ImDrawCornerFlags_All);
    });
}
void DrawQueue::queue_circle(ImVec2 center, float radius, float thickness, ImU32 color)
{
    m_Queue.emplace_back([=](ImDrawList* draw_list) {
        draw_list->AddCircle(center, radius, color, 0, thickness);
    });
}
void DrawQueue::queue_circle_filled(ImVec2 center, float radius, ImU32 color)
{
    m_Queue.emplace_back([=](ImDrawList* draw_list) {
        draw_list->AddCircleFilled(center, radius, color, 0);
    });
}
void DrawQueue::queue_text(ImVec2 start, std::string text, ImU32 color)
{
    m_Queue.emplace_back([=, text = std::move(text)](ImDrawList* draw_list) {
        draw_list->AddText(start, color, text.c_str());
    });
}

void DrawQueue::draw(ImDrawList* draw_list)
{
	for (auto& queue_element : m_Queue)
	{
        queue_element(draw_list);
	}
}
void DrawQueue::clear()
{
	m_Queue.clear();
}
