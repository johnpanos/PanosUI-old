#include <iostream>
#include <vector>

class EventResponder {
public:
    EventResponder *next;

    virtual void respond() {
        std::cout << "default\n";
    };
};

// class UILayer {
//     virtual void render(cairo_surface_t *surface) {

//     };
// };

class UIView: public EventResponder {
public:
    std::vector<UIView*> children {};

    virtual void render() {
        std::cout << "render\n";
    };

    void addSubView(UIView *view) {
        this->children.push_back(view);
        view->next = this;
    }
};

class MyView: public UIView {
public:
    void respond() {
        this->next->respond();
    }
};

int main() {
    std::cout << "Hello World\n";

    UIView *view1 = new UIView;
    MyView *view2 = new MyView;

    EventResponder *er = dynamic_cast<EventResponder*>(view2);

    view1->addSubView(view2);

    er->respond();

    view2->next->respond();
}