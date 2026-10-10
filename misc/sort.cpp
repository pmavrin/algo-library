sort(a.begin(), a.end(),
[](const auto & a, const auto & b) -> bool
{
return a.mProperty > b.mProperty;
});